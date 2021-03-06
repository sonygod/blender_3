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
 * The Original Code is Copyright (C) 2011 by Bastien Montagne.
 * All rights reserved.
 */

/** \file
 * \ingroup modifiers
 */

#pragma once

struct CurveMapping;
struct MDeformVert;
struct MDeformWeight;
struct Mesh;
struct ModifierEvalContext;
struct Object;
struct PointerRNA;
struct RNG;
struct Scene;
struct Tex;
struct uiLayout;

/*
 * XXX I'd like to make modified weights visible in WeightPaint mode,
 *     but couldn't figure a way to do this...
 *     Maybe this will need changes in mesh_calc_modifiers?
 *     Or the WeightPaint mode code itself?
 */

/**************************************
 * Util functions.                    *
 **************************************/

/**
 * We cannot divide by zero (what a surprise...).
 * So if `-MOD_WEIGHTVGROUP_DIVMODE_ZEROFLOOR < weightf < MOD_WEIGHTVGROUP_DIVMODE_ZEROFLOOR`,
 * we clamp weightf to this value (or its negative version).
 * Also used to avoid null power factor.
 */
#define MOD_WVG_ZEROFLOOR 1.0e-32f

/**
 * Maps new_w weights in place, using either one of the predefined functions, or a custom curve.
 * Return values are in new_w.
 * If indices is not NULL, it must be a table of same length as org_w and new_w,
 * mapping to the real vertex index (in case the weight tables do not cover the whole vertices...).
 * cmap might be NULL, in which case curve mapping mode will return unmodified data.
 */
void weightvg_do_map(int num,
                     float *new_w,
                     short falloff_type,
                     const bool do_invert,
                     struct CurveMapping *cmap,
                     struct RNG *rng);

/**
 * Applies new_w weights to org_w ones, using either a texture, vgroup or constant value as factor.
 * Return values are in org_w.
 * If indices is not NULL, it must be a table of same length as org_w and new_w,
 * mapping to the real vertex index (in case the weight tables do not cover the whole vertices...).
 * XXX The standard "factor" value is assumed in [0.0, 1.0] range.
 * Else, weird results might appear.
 */
void weightvg_do_mask(const ModifierEvalContext *ctx,
                      const int num,
                      const int *indices,
                      float *org_w,
                      const float *new_w,
                      Object *ob,
                      struct Mesh *mesh,
                      const float fact,
                      const char defgrp_name[MAX_VGROUP_NAME],
                      struct Scene *scene,
                      Tex *texture,
                      const int tex_use_channel,
                      const int tex_mapping,
                      Object *tex_map_object,
                      const char *text_map_bone,
                      const char *tex_uvlayer_name,
                      const bool invert_vgroup_mask);

/**
 * Applies weights to given vgroup (defgroup), and optionally add/remove vertices from the group.
 * If dws is not NULL, it must be an array of #MDeformWeight pointers of same length as weights
 * (and defgrp_idx can then have any value).
 * If indices is not NULL, it must be an array of same length as weights, mapping to the real
 * vertex index (in case the weight array does not cover the whole vertices...).
 */
void weightvg_update_vg(struct MDeformVert *dvert,
                        int defgrp_idx,
                        struct MDeformWeight **dws,
                        int num,
                        const int *indices,
                        const float *weights,
                        const bool do_add,
                        const float add_thresh,
                        const bool do_rem,
                        const float rem_thresh,
                        const bool do_normalize);

/**
 * Common vertex weight mask interface elements for the modifier panels.
 */
void weightvg_ui_common(const bContext *C, PointerRNA *ob_ptr, PointerRNA *ptr, uiLayout *layout);
