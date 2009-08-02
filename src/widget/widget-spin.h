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
 * \addtogroup liwdgSpin Spin
 * @{
 */

#ifndef __WIDGET_SPIN_H__
#define __WIDGET_SPIN_H__

#include <font/lips-font.h>
#include "widget.h"

#define LIWDG_SPIN(o) ((liwdgSpin*)(o))

typedef struct _liwdgSpin liwdgSpin;
struct _liwdgSpin
{
	liwdgWidget base;
	lifntFont* font;
	lifntLayout* text;
	float value;
};

extern const liwdgClass liwdgSpinType;

liwdgWidget*
liwdg_spin_new (liwdgManager* manager);

lifntFont*
liwdg_spin_get_font (liwdgSpin* self);

void
liwdg_spin_set_font (liwdgSpin* self,
                     lifntFont* font);

float
liwdg_spin_get_value (liwdgSpin* self);

void
liwdg_spin_set_value (liwdgSpin* self,
                      float      value);

#endif

/** @} */
/** @} */
