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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliVoxel Voxel
 * @{
 */

#include "ext-module.h"
#include "ext-block.h"

LIExtBlock*
liext_block_new (LIExtModule* module)
{
	LIExtBlock* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtBlock));
	if (self == NULL)
		return NULL;
	self->module = module;

	return self;
}

void
liext_block_free (LIExtBlock* self)
{
	if (self->group != NULL)
		liren_group_free (self->group);
	if (self->rmodel != NULL)
		liren_model_free (self->rmodel);
	if (self->mmodel != NULL)
		limdl_model_free (self->mmodel);
	if (self->physics != NULL)
		liphy_object_free (self->physics);
	lisys_free (self);
}

void
liext_block_clear (LIExtBlock* self)
{
	if (self->group != NULL)
	{
		liren_group_free (self->group);
		self->group = NULL;
	}
	if (self->rmodel != NULL)
	{
		liren_model_free (self->rmodel);
		self->rmodel = NULL;
	}
	if (self->mmodel != NULL)
	{
		limdl_model_free (self->mmodel);
		self->mmodel = NULL;
	}
	if (self->physics != NULL)
	{
		liphy_object_free (self->physics);
		self->physics = NULL;
	}
}

int
liext_block_build (LIExtBlock*     self,
                   LIVoxBlockAddr* addr)
{
	LIMatTransform transform;

	/* Free old objects. */
	liext_block_clear (self);

	/* Build new objects. */
	if (!livox_build_block (self->module->voxels, self->module->client->engine, addr, &self->mmodel, &self->physics))
		return 0;

	/* Create render model if not empty. */
	if (self->mmodel != NULL)
	{
		self->rmodel = liren_model_new (self->module->client->render, self->mmodel, NULL);
		if (self->rmodel != NULL)
		{
			self->group = liren_group_new (self->module->client->scene);
			if (self->group != NULL)
			{
				transform = limat_transform_identity ();
				liren_group_insert_model (self->group, self->rmodel, &transform);
			}
		}
	}

	/* Realize if not empty. */
	if (self->group != NULL)
		liren_group_set_realized (self->group, 1);
	if (self->physics != NULL)
		liphy_object_set_realized (self->physics, 1);

	return 1;
}

/** @} */
/** @} */
/** @} */
