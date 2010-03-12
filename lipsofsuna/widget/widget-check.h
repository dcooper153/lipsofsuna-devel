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
 * \addtogroup LIWdgCheck Check
 * @{
 */

#ifndef __WIDGET_CHECK_H__
#define __WIDGET_CHECK_H__

#include <lipsofsuna/font.h>
#include <lipsofsuna/system.h>
#include "widget.h"

#define LIWDG_CHECK(o) ((LIWdgCheck*)(o))

/**
 * \brief Checkbox widget.
 */
typedef struct _LIWdgCheck LIWdgCheck;
struct _LIWdgCheck
{
	LIWdgWidget base;
	LIFntFont* font;
	LIFntLayout* text;
	int active;
	char* string;
};

LIAPICALL (const LIWdgClass*, liwdg_widget_check, ());

LIAPICALL (LIWdgWidget*, liwdg_check_new, (
	LIWdgManager* manager));

LIAPICALL (int, liwdg_check_get_active, (
	LIWdgCheck* self));

LIAPICALL (void, liwdg_check_set_active, (
	LIWdgCheck* self,
	int         active));

LIAPICALL (LIFntFont*, liwdg_check_get_font, (
	LIWdgCheck* self));

LIAPICALL (void, liwdg_check_set_font, (
	LIWdgCheck* self,
	LIFntFont*  font));

LIAPICALL (int, liwdg_check_set_text, (
	LIWdgCheck* self,
	const char* text));

LIAPICALL (const char*, liwdg_check_get_text, (
	LIWdgCheck* self));

#endif

/** @} */
/** @} */
