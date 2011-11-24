/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenImage Image
 * @{
 */

#include "render-internal.h"

/**
 * \brief Creates an empty image.
 * \param render Renderer.
 * \param name Image name.
 * \return Image or NULL.
 */
LIRenImage* liren_image_new (
	LIRenRender* render,
	const char*  name)
{
	LIRenImage* self;

	self = lisys_calloc (1, sizeof (LIRenImage));
	if (self == NULL)
		return NULL;
	self->name = lisys_string_dup (name);
	self->render = render;
	self->timestamp = lisys_time (NULL);

	/* Initialize the backend. */
	/* TODO */

	/* Add to dictionary. */
	if (!lialg_strdic_insert (self->render->images, name, self))
	{
		liren_image_free (self);
		return 0;
	}

	return self;
}

/**
 * \brief Frees the image.
 * \param self Image.
 */
void liren_image_free (
	LIRenImage* self)
{
	lisys_assert (!self->refs);
	lialg_strdic_remove (self->render->images, self->name);
	/* TODO */
	lisys_free (self->name);
	lisys_free (self);
}

void liren_image_ref (
	LIRenImage* self)
{
	self->refs++;
}

void liren_image_unref (
	LIRenImage* self)
{
	lisys_assert (self->refs > 0);
	self->refs--;
	self->timestamp = lisys_time (NULL);
}

GLuint liren_image_get_handle (
	const LIRenImage* self)
{
	/* TODO */
	return 0;
}

int liren_image_get_height (
	const LIRenImage* self)
{
	/* TODO */
	return 128;
}

int liren_image_get_width (
	const LIRenImage* self)
{
	/* TODO */
	return 128;
}

/** @} */
/** @} */
/** @} */
