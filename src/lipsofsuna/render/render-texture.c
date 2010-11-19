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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenTexture Texture
 * @{
 */

#include "render-texture.h"

void liren_texture_init (
	LIRenTexture* self)
{
	memset (self, 0, sizeof (LIRenTexture));
}

void liren_texture_free (
	LIRenTexture* self)
{
}

void liren_texture_set_image (
	LIRenTexture* self,
	LIRenImage*   value)
{
	self->image = value;
	if (value != NULL && value->texture != NULL)
		self->texture = value->texture->texture;
	else
		self->texture = 0;
}

/** @} */
/** @} */
