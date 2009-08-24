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
 * \addtogroup liwdgStyle Style
 * @{
 */

#ifndef __WIDGET_STYLE_H__
#define __WIDGET_STYLE_H__

#include <image/lips-image.h>
#include "widget-types.h"

struct _liwdgStyle
{
	liimgTexture* texture;
	int x, y;
	int w[3];
	int h[3];
	int pad[4];
	float color[4];
	float selection[4];
};

struct _liwdgStyles
{
	liwdgManager* manager;
	liwdgStyle fallback;
	lialgStrdic* fonts;
	lialgStrdic* images;
	lialgStrdic* subimgs;
};

liwdgStyles*
liwdg_styles_new (liwdgManager* manager,
                  const char*   root);

void
liwdg_styles_free (liwdgStyles* self);

#endif

/** @} */
/** @} */
