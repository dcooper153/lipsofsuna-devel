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
 * \addtogroup liwdgCheck Check
 * @{
 */

#ifndef __WIDGET_CHECK_H__
#define __WIDGET_CHECK_H__

#include <font/lips-font.h>
#include "widget.h"

#define LIWDG_CHECK(o) ((liwdgCheck*)(o))

/**
 * \brief Checkbox widget.
 */
typedef struct _liwdgCheck liwdgCheck;
struct _liwdgCheck
{
	liwdgWidget base;
	lifntFont* font;
	lifntLayout* text;
	int active;
	char* string;
};

extern const liwdgWidgetClass liwdgCheckType;

liwdgWidget*
liwdg_check_new (liwdgManager* manager);

int
liwdg_check_get_active (liwdgCheck* self);

void
liwdg_check_set_active (liwdgCheck* self,
                        int         active);

lifntFont*
liwdg_check_get_font (liwdgCheck* self);

void
liwdg_check_set_font (liwdgCheck* self,
                      lifntFont*  font);

int
liwdg_check_set_text (liwdgCheck* self,
                      const char* text);

const char*
liwdg_check_get_text (liwdgCheck* self);

#endif

/** @} */
/** @} */
