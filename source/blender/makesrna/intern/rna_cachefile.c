/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2016 Blender Foundation.
 * All rights reserved.
 */

/** \file
 * \ingroup RNA
 */

#include "DNA_cachefile_types.h"
#include "DNA_scene_types.h"

#include "RNA_access.h"
#include "RNA_define.h"
#include "RNA_enum_types.h"

#include "rna_internal.h"

#ifdef RNA_RUNTIME

#  include "BLI_string.h"

#  include "BKE_cachefile.h"

#  include "DEG_depsgraph.h"
#  include "DEG_depsgraph_build.h"

#  include "WM_api.h"
#  include "WM_types.h"

#  ifdef WITH_ALEMBIC
#    include "ABC_alembic.h"
#  endif

static void rna_CacheFile_update(Main *UNUSED(bmain), Scene *UNUSED(scene), PointerRNA *ptr)
{
  CacheFile *cache_file = (CacheFile *)ptr->data;

  DEG_id_tag_update(&cache_file->id, ID_RECALC_COPY_ON_WRITE);
  WM_main_add_notifier(NC_OBJECT | ND_DRAW, NULL);
}

static void rna_CacheFile_dependency_update(Main *bmain, Scene *scene, PointerRNA *ptr)
{
  rna_CacheFile_update(bmain, scene, ptr);
  DEG_relations_tag_update(bmain);
}

static void rna_CacheFile_object_paths_begin(CollectionPropertyIterator *iter, PointerRNA *ptr)
{
  CacheFile *cache_file = (CacheFile *)ptr->data;
  rna_iterator_listbase_begin(iter, &cache_file->object_paths, NULL);
}

#else

/* cachefile.object_paths */
static void rna_def_alembic_object_path(BlenderRNA *brna)
{
  StructRNA *srna = RNA_def_struct(brna, "CacheObjectPath", NULL);
  RNA_def_struct_sdna(srna, "CacheObjectPath");
  RNA_def_struct_ui_text(srna, "Object Path", "Path of an object inside of an Alembic archive");
  RNA_def_struct_ui_icon(srna, ICON_NONE);

  RNA_define_lib_overridable(true);

  PropertyRNA *prop = RNA_def_property(srna, "path", PROP_STRING, PROP_NONE);
  RNA_def_property_ui_text(prop, "Path", "Object path");
  RNA_def_struct_name_property(srna, prop);

  RNA_define_lib_overridable(false);
}

/* cachefile.object_paths */
static void rna_def_cachefile_object_paths(BlenderRNA *brna, PropertyRNA *cprop)
{
  RNA_def_property_srna(cprop, "CacheObjectPaths");
  StructRNA *srna = RNA_def_struct(brna, "CacheObjectPaths", NULL);
  RNA_def_struct_sdna(srna, "CacheFile");
  RNA_def_struct_ui_text(srna, "Object Paths", "Collection of object paths");
}

static void rna_def_cachefile(BlenderRNA *brna)
{
  StructRNA *srna = RNA_def_struct(brna, "CacheFile", "ID");
  RNA_def_struct_sdna(srna, "CacheFile");
  RNA_def_struct_ui_text(srna, "CacheFile", "");
  RNA_def_struct_ui_icon(srna, ICON_FILE);

  RNA_define_lib_overridable(true);

  PropertyRNA *prop = RNA_def_property(srna, "filepath", PROP_STRING, PROP_FILEPATH);
  RNA_def_property_ui_text(prop, "File Path", "Path to external displacements file");
  RNA_def_property_update(prop, 0, "rna_CacheFile_update");

  prop = RNA_def_property(srna, "is_sequence", PROP_BOOLEAN, PROP_NONE);
  RNA_def_property_ui_text(
      prop, "Sequence", "Whether the cache is separated in a series of files");
  RNA_def_property_update(prop, 0, "rna_CacheFile_update");

  prop = RNA_def_property(srna, "use_render_procedural", PROP_BOOLEAN, PROP_NONE);
  RNA_def_property_ui_text(
      prop,
      "Use Render Engine Procedural",
      "Display boxes in the viewport as placeholders for the objects, Cycles will use a "
      "procedural to load the objects during viewport rendering in experimental mode, "
      "other render engines will also receive a placeholder and should take care of loading the "
      "Alembic data themselves if possible");
  RNA_def_property_update(prop, 0, "rna_CacheFile_dependency_update");

  /* ----------------- For Scene time ------------------- */

  prop = RNA_def_property(srna, "override_frame", PROP_BOOLEAN, PROP_NONE);
  RNA_def_property_ui_text(prop,
                           "Override Frame",
                           "Whether to use a custom frame for looking up data in the cache file,"
                           " instead of using the current scene frame");
  RNA_def_property_update(prop, 0, "rna_CacheFile_update");

  prop = RNA_def_property(srna, "frame", PROP_FLOAT, PROP_NONE);
  RNA_def_property_float_sdna(prop, NULL, "frame");
  RNA_def_property_range(prop, -MAXFRAME, MAXFRAME);
  RNA_def_property_ui_text(prop,
                           "Frame",
                           "The time to use for looking up the data in the cache file,"
                           " or to determine which file to use in a file sequence");
  RNA_def_property_update(prop, 0, "rna_CacheFile_update");

  prop = RNA_def_property(srna, "frame_offset", PROP_FLOAT, PROP_NONE);
  RNA_def_property_float_sdna(prop, NULL, "frame_offset");
  RNA_def_property_range(prop, -MAXFRAME, MAXFRAME);
  RNA_def_property_ui_text(prop,
                           "Frame Offset",
                           "Subtracted from the current frame to use for "
                           "looking up the data in the cache file, or to "
                           "determine which file to use in a file sequence");
  RNA_def_property_update(prop, 0, "rna_CacheFile_update");

  /* ----------------- Cache controls ----------------- */

  prop = RNA_def_property(srna, "use_prefetch", PROP_BOOLEAN, PROP_NONE);
  RNA_def_property_ui_text(
      prop,
      "Use Prefetch",
      "When enabled, the Cycles Procedural will preload animation data for faster updates");
  RNA_def_property_update(prop, 0, "rna_CacheFile_update");

  prop = RNA_def_property(srna, "prefetch_cache_size", PROP_INT, PROP_UNSIGNED);
  RNA_def_property_ui_text(
      prop,
      "Prefetch Cache Size",
      "Memory usage limit in megabytes for the Cycles Procedural cache, if the data does not "
      "fit within the limit, rendering is aborted");
  RNA_def_property_update(prop, 0, "rna_CacheFile_update");

  /* ----------------- Axis Conversion ----------------- */

  prop = RNA_def_property(srna, "forward_axis", PROP_ENUM, PROP_NONE);
  RNA_def_property_enum_sdna(prop, NULL, "forward_axis");
  RNA_def_property_enum_items(prop, rna_enum_object_axis_items);
  RNA_def_property_ui_text(prop, "Forward", "");
  RNA_def_property_update(prop, 0, "rna_CacheFile_update");

  prop = RNA_def_property(srna, "up_axis", PROP_ENUM, PROP_NONE);
  RNA_def_property_enum_sdna(prop, NULL, "up_axis");
  RNA_def_property_enum_items(prop, rna_enum_object_axis_items);
  RNA_def_property_ui_text(prop, "Up", "");
  RNA_def_property_update(prop, 0, "rna_CacheFile_update");

  prop = RNA_def_property(srna, "scale", PROP_FLOAT, PROP_NONE);
  RNA_def_property_float_sdna(prop, NULL, "scale");
  RNA_def_property_range(prop, 0.0001f, 1000.0f);
  RNA_def_property_ui_text(
      prop,
      "Scale",
      "Value by which to enlarge or shrink the object with respect to the world's origin"
      " (only applicable through a Transform Cache constraint)");
  RNA_def_property_update(prop, 0, "rna_CacheFile_update");

  /* object paths */
  prop = RNA_def_property(srna, "object_paths", PROP_COLLECTION, PROP_NONE);
  RNA_def_property_collection_sdna(prop, NULL, "object_paths", NULL);
  RNA_def_property_collection_funcs(prop,
                                    "rna_CacheFile_object_paths_begin",
                                    "rna_iterator_listbase_next",
                                    "rna_iterator_listbase_end",
                                    "rna_iterator_listbase_get",
                                    NULL,
                                    NULL,
                                    NULL,
                                    NULL);
  RNA_def_property_struct_type(prop, "CacheObjectPath");
  RNA_def_property_srna(prop, "CacheObjectPaths");
  RNA_def_property_ui_text(
      prop, "Object Paths", "Paths of the objects inside the Alembic archive");

  /* ----------------- Alembic Velocity Attribute ----------------- */

  prop = RNA_def_property(srna, "velocity_name", PROP_STRING, PROP_NONE);
  RNA_def_property_ui_text(prop,
                           "Velocity Attribute",
                           "Name of the Alembic attribute used for generating motion blur data");
  RNA_def_property_update(prop, 0, "rna_CacheFile_update");
  RNA_def_property_clear_flag(prop, PROP_ANIMATABLE);

  static const EnumPropertyItem velocity_unit_items[] = {
      {CACHEFILE_VELOCITY_UNIT_SECOND, "SECOND", 0, "Second", ""},
      {CACHEFILE_VELOCITY_UNIT_FRAME, "FRAME", 0, "Frame", ""},
      {0, NULL, 0, NULL, NULL},
  };

  prop = RNA_def_property(srna, "velocity_unit", PROP_ENUM, PROP_NONE);
  RNA_def_property_enum_sdna(prop, NULL, "velocity_unit");
  RNA_def_property_enum_items(prop, velocity_unit_items);
  RNA_def_property_ui_text(
      prop,
      "Velocity Unit",
      "Define how the velocity vectors are interpreted with regard to time, 'frame' means "
      "the delta time is 1 frame, 'second' means the delta time is 1 / FPS");
  RNA_def_property_update(prop, 0, "rna_CacheFile_update");
  RNA_def_property_clear_flag(prop, PROP_ANIMATABLE);

  RNA_define_lib_overridable(false);

  rna_def_cachefile_object_paths(brna, prop);

  rna_def_animdata_common(srna);
}

void RNA_def_cachefile(BlenderRNA *brna)
{
  rna_def_cachefile(brna);
  rna_def_alembic_object_path(brna);
}

#endif
