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
 * \addtogroup LIWdgSpin Spin
 * @{
 */

#ifndef __WIDGET_SPIN_H__
#define __WIDGET_SPIN_H__

#include <lipsofsuna/font.h>
#include "widget.h"

#define LIWDG_SPIN(o) ((LIWdgSpin*)(o))

typedef struct _LIWdgSpin LIWdgSpin;
struct _LIWdgSpin
{
	LIWdgWidget base;
	LIFntFont* font;
	LIFntLayout* text;
	float value;
};

extern const LIWdgClass liwdg_widget_spin;

LIWdgWidget*
liwdg_spin_new (LIWdgManager* manager);

LIFntFont*
liwdg_spin_get_font (LIWdgSpin* self);

void
liwdg_spin_set_font (LIWdgSpin* self,
                     LIFntFont* font);

float
liwdg_spin_get_value (LIWdgSpin* self);

void
liwdg_spin_set_value (LIWdgSpin* self,
                      float      value);

#endif

/** @} */
/** @} */
