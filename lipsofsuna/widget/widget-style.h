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
 * \addtogroup LIWdgStyle Style
 * @{
 */

#ifndef __WIDGET_STYLE_H__
#define __WIDGET_STYLE_H__

#include <lipsofsuna/image.h>
#include <lipsofsuna/system.h>
#include "widget-types.h"

struct _LIWdgStyle
{
	LIImgTexture* texture;
	int x, y;
	int w[3];
	int h[3];
	int pad[4];
	float color[4];
	float hover[4];
	float selection[4];
};

struct _LIWdgStyles
{
	LIWdgManager* manager;
	LIWdgStyle fallback;
	LIAlgStrdic* fonts;
	LIAlgStrdic* images;
	LIAlgStrdic* subimgs;
};

LIAPICALL (LIWdgStyles*, liwdg_styles_new, (
	LIWdgManager* manager,
	const char*   root));

LIAPICALL (void, liwdg_styles_free, (
	LIWdgStyles* self));

#endif

/** @} */
/** @} */
