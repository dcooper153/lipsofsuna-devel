/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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
 * \addtogroup LIWdgProgress Progress
 * @{
 */

#ifndef __WIDGET_PROGRESS_H__
#define __WIDGET_PROGRESS_H__

#include <lipsofsuna/font.h>
#include <lipsofsuna/system.h>
#include "widget.h"

#define LIWDG_PROGRESS(o) ((LIWdgProgress*)(o))

typedef struct _LIWdgProgress LIWdgProgress;
struct _LIWdgProgress
{
	LIWdgWidget base;
	LIFntFont* font;
	LIFntLayout* text;
	char* string;
	float value;
};

LIAPICALL (const LIWdgClass*, liwdg_widget_progress, ());

LIAPICALL (LIWdgWidget*, liwdg_progress_new, (
	LIWdgManager* manager));

LIAPICALL (LIFntFont*, liwdg_progress_get_font, (
	LIWdgProgress* self));

LIAPICALL (void, liwdg_progress_set_font, (
	LIWdgProgress* self,
	LIFntFont*     font));

LIAPICALL (int, liwdg_progress_set_text, (
	LIWdgProgress* self,
	const char*    text));

LIAPICALL (const char*, liwdg_progress_get_text, (
	LIWdgProgress* self));

LIAPICALL (float, liwdg_progress_get_value, (
	LIWdgProgress* self));

LIAPICALL (void, liwdg_progress_set_value, (
	LIWdgProgress* self,
	float          value));

#endif

/** @} */
/** @} */
