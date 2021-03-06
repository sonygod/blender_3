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
 * The Original Code is Copyright (C) 2013, Blender Foundation
 */

/** \file
 * \ingroup editors
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct ScrArea;
struct SpaceProperties;
struct bContext;

/**
 * Fills an array with the tab context values for the properties editor. -1 signals a separator.
 *
 * \return The total number of items in the array returned.
 */
int ED_buttons_tabs_list(struct SpaceProperties *sbuts, short *context_tabs_array);
bool ED_buttons_tab_has_search_result(struct SpaceProperties *sbuts, const int index);

void ED_buttons_search_string_set(struct SpaceProperties *sbuts, const char *value);
int ED_buttons_search_string_length(struct SpaceProperties *sbuts);
const char *ED_buttons_search_string_get(struct SpaceProperties *sbuts);

bool ED_buttons_should_sync_with_outliner(const struct bContext *C,
                                          const struct SpaceProperties *sbuts,
                                          struct ScrArea *area);
void ED_buttons_set_context(const struct bContext *C,
                            struct SpaceProperties *sbuts,
                            PointerRNA *ptr,
                            const int context);

#ifdef __cplusplus
}
#endif
