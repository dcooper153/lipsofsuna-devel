/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
 *
 * Lips of Suna is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Lips of Suna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lips of Suna. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \addtogroup liwdg Widget
 * @{
 * \addtogroup liwdgProgress Progress
 * @{
 */

#ifndef __WIDGET_PROGRESS_H__
#define __WIDGET_PROGRESS_H__

#include <font/lips-font.h>
#include "widget.h"

#define LIWDG_PROGRESS(o) ((liwdgProgress*)(o))

typedef struct _liwdgProgress liwdgProgress;
struct _liwdgProgress
{
	liwdgWidget base;
	lifntFont* font;
	lifntLayout* text;
	char* string;
	float value;
};

extern const liwdgClass liwdgProgressType;

liwdgWidget*
liwdg_progress_new (liwdgManager* manager);

lifntFont*
liwdg_progress_get_font (liwdgProgress* self);

void
liwdg_progress_set_font (liwdgProgress* self,
                         lifntFont*     font);

int
liwdg_progress_set_text (liwdgProgress* self,
                         const char*    text);

const char*
liwdg_progress_get_text (liwdgProgress* self);

float
liwdg_progress_get_value (liwdgProgress* self);

void
liwdg_progress_set_value (liwdgProgress* self,
                          float          value);

#endif

/** @} */
/** @} */
