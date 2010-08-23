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
 * \addtogroup livox Voxel
 * @{
 * \addtogroup LIVoxMaterial Material
 * @{
 */

#include <lipsofsuna/string.h>
#include "voxel-material.h"

/**
 * \brief Creates a new material.
 *
 * \return New material or NULL.
 */
LIVoxMaterial*
livox_material_new ()
{
	int flags;
	LIVoxMaterial* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIVoxMaterial));
	if (self == NULL)
		return NULL;
	self->type = LIVOX_MATERIAL_TYPE_TILE;

	/* Allocate name. */
	self->name = listr_dup ("");
	if (self->name == NULL)
	{
		livox_material_free (self);
		return NULL;
	}

	/* Allocate materials. */
	flags = LIMDL_TEXTURE_FLAG_BILINEAR | LIMDL_TEXTURE_FLAG_MIPMAP | LIMDL_TEXTURE_FLAG_REPEAT;
	if (!limdl_material_init (&self->mat_side) ||
	    !limdl_material_init (&self->mat_top) ||
	    !limdl_material_append_texture (&self->mat_side, LIMDL_TEXTURE_TYPE_IMAGE, flags, "stone-000") ||
	    !limdl_material_append_texture (&self->mat_top, LIMDL_TEXTURE_TYPE_IMAGE, flags, "grass-000"))
	{
		livox_material_free (self);
		return NULL;
	}

	/* FIXME: Abusing for texture scaling... */
	self->mat_side.emission = 0.9f;
	self->mat_top.emission = 0.9f;

	return self;
}

/**
 * \brief Creates a copy of a material.
 *
 * \param src Material to copy.
 * \return Soft copy of the material or NULL.
 */
LIVoxMaterial*
livox_material_new_copy (const LIVoxMaterial* src)
{
	LIVoxMaterial* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIVoxMaterial));
	if (self == NULL)
		return NULL;

	/* Copy values. */
	self->id = src->id;
	self->flags = src->flags;
	self->type = src->type;
	self->friction = src->friction;

	/* Copy name. */
	self->name = listr_dup (src->name);
	if (self->name == NULL)
	{
		livox_material_free (self);
		return NULL;
	}

	/* Copy model. */
	self->model = src->model;

	/* Copy materials. */
	if (!limdl_material_init_copy (&self->mat_side, &src->mat_side) ||
	    !limdl_material_init_copy (&self->mat_top, &src->mat_top))
	{
		livox_material_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the material.
 *
 * \param self Material.
 */
void
livox_material_free (LIVoxMaterial* self)
{
	limdl_material_free (&self->mat_side);
	limdl_material_free (&self->mat_top);
	lisys_free (self->name);
	lisys_free (self);
}

int
livox_material_set_name (LIVoxMaterial* self,
                         const char*    value)
{
	char* tmp;

	tmp = listr_dup (value);
	if (tmp == NULL)
		return 0;
	lisys_free (self->name);
	self->name = tmp;

	return 1;
}

/**
 * \brief Sets the tile model of the material.
 *
 * The caller retains the ownership of the model and needs to ensure that the
 * model isn't freed until either the model of the material is changed or the
 * material is deleted.
 *
 * \param self Material.
 * \param model Model.
 * \return Nonzero on success.
 */
int livox_material_set_model (
	LIVoxMaterial* self,
	LIEngModel*    model)
{
	self->model = model;

	return 1;
}

/** @} */
/** @} */
