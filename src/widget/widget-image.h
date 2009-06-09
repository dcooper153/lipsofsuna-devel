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
 * \addtogroup liwdgImage Image
 * @{
 */

#ifndef __WIDGET_IMAGE_H__
#define __WIDGET_IMAGE_H__

#include "widget.h"

#define LIWDG_IMAGE(o) ((liwdgImage*)(o))

/**
 * \brief Image widget.
 */
typedef struct _liwdgImage liwdgImage;
struct _liwdgImage
{
	liwdgWidget base;
	char* image;
};

extern const liwdgClass liwdgImageType;

liwdgWidget*
liwdg_image_new (liwdgManager* manager);

int
liwdg_image_set_image (liwdgImage* self,
                       const char*  text);

const char*
liwdg_image_get_image (liwdgImage* self);

#endif

/** @} */
/** @} */
