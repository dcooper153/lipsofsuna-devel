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
 * \addtogroup liren Render
 * @{
 * \addtogroup LIRenObject Object
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/system.h>
#include "render-group.h"
#include "render-scene.h"

static void
private_build_bounds (LIRenGroup*       self,
                      LIRenGroupObject* object,
                      int               first);

static void
private_build_lights (LIRenGroup*       self,
                      LIRenGroupObject* object);

/*****************************************************************************/

/**
 * \brief Creates a new render group and adds it to the scene.
 *
 * \param scene Scene.
 * \return New group or NULL.
 */
LIRenGroup*
liren_group_new (LIRenScene* scene)
{
	LIRenGroup* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenGroup));
	if (self == NULL)
		return NULL;
	self->scene = scene;
	limat_aabb_init (&self->aabb);

	/* Add to renderer. */
	if (!lialg_ptrdic_insert (scene->groups, self, self))
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the render group.
 *
 * \param self Group.
 */
void
liren_group_free (LIRenGroup* self)
{
	int i;
	LIRenGroupObject* object;
	LIRenGroupObject* next;

	for (object = self->objects ; object != NULL ; object = next)
	{
		next = object->next;
		lisys_free (object);
	}
	for (i = 0 ; i < self->lights.count ; i++)
	{
		if (self->realized)
			liren_lighting_remove_light (self->scene->lighting, self->lights.array[i]);
		liren_light_free (self->lights.array[i]);
	}
	lialg_ptrdic_remove (self->scene->groups, self);
	lisys_free (self->lights.array);
	lisys_free (self);
}

/**
 * \brief Removes all models from the group.
 *
 * \param self Group.
 */
void
liren_group_clear (LIRenGroup* self)
{
	int i;
	LIRenGroupObject* object;
	LIRenGroupObject* next;

	for (object = self->objects ; object != NULL ; object = next)
	{
		next = object->next;
		lisys_free (object);
	}
	for (i = 0 ; i < self->lights.count ; i++)
	{
		if (self->realized)
			liren_lighting_remove_light (self->scene->lighting, self->lights.array[i]);
		liren_light_free (self->lights.array[i]);
	}
	lisys_free (self->lights.array);
	self->lights.array = NULL;
	self->lights.count = 0;
	limat_aabb_init (&self->aabb);
	self->objects = NULL;
}

/**
 * \brief Inserts a model to the group.
 *
 * \param self Group.
 * \param model Model.
 * \param transform Model transformation.
 * \return Nonzero on success.
 */
int
liren_group_insert_model (LIRenGroup*     self,
                          LIRenModel*     model,
                          LIMatTransform* transform)
{
	LIRenGroupObject* object;

	/* Allocate object. */
	object = lisys_calloc (1, sizeof (LIRenGroupObject));
	if (object == NULL)
		return 0;
	object->model = model;
	object->transform = *transform;

	/* Calculate bounds. */
	private_build_bounds (self, object, self->objects == NULL);

	/* Add to list. */
	object->next = self->objects;
	self->objects = object;
	private_build_lights (self, object);

	return 1;
}

/**
 * \brief Called when a model is being reloaded.
 *
 * Replaces all instances of the old model with the new model and rebuilds
 * the light list of the group if any of the state of the group was changed.
 *
 * \param self Group.
 * \param model_old Model to be deleted.
 * \param model_new Model that has been loaded as a replacement.
 */
void
liren_group_reload_model (LIRenGroup* self,
                          LIRenModel* model_old,
                          LIRenModel* model_new)
{
	int i;
	int found = 0;
	LIRenGroupObject* object;

	/* Replace objects. */
	for (object = self->objects ; object != NULL ; object = object->next)
	{
		if (object->model == model_old)
		{
			object->model = model_new;
			found = 1;
		}
	}
	if (!found)
		return;

	/* Recalculate bounds. */
	for (object = self->objects ; object != NULL ; object = object->next)
		private_build_bounds (self, object, object == self->objects);

	/* Remove old lights. */
	for (i = 0 ; i < self->lights.count ; i++)
	{
		if (self->realized)
			liren_lighting_remove_light (self->scene->lighting, self->lights.array[i]);
		liren_light_free (self->lights.array[i]);
	}
	lisys_free (self->lights.array);
	self->lights.array = NULL;
	self->lights.count = 0;

	/* Create new lights. */
	for (object = self->objects ; object != NULL ; object = object->next)
		private_build_lights (self, object);
}

/**
 * \brief Advances the timer of the group.
 *
 * \param self Group.
 * \param secs Number of seconds since the last update.
 */
void
liren_group_update (LIRenGroup* self,
                    float       secs)
{
	/* FIXME: We don't need this currently but someone else might. */
}

/**
 * \brief Gets the bounding box of the group.
 *
 * \param self Group.
 * \param result Return location for the bounding box.
 */
void
liren_group_get_bounds (const LIRenGroup* self,
                        LIMatAabb*        result)
{
	*result = self->aabb;
}

/**
 * \brief Checks if the group is renderable.
 *
 * Returns nonzero if the realized flag of the group is set.
 *
 * \param self Group.
 * \return Nonzero if realized.
 */
int
liren_group_get_realized (const LIRenGroup* self)
{
	return self->realized && self->objects != NULL;
}

/**
 * \brief Sets the realized flag of the group.
 *
 * \param self Group.
 * \param value Flag value.
 * \return Nonzero if succeeded.
 */
int
liren_group_set_realized (LIRenGroup* self,
                          int         value)
{
	int i;

	/* Register or unregister light sources. */
	if (!self->realized && value)
	{
		for (i = 0 ; i < self->lights.count ; i++)
			liren_lighting_insert_light (self->scene->lighting, self->lights.array[i]);
	}
	else if (self->realized && !value)
	{
		for (i = 0 ; i < self->lights.count ; i++)
			liren_lighting_remove_light (self->scene->lighting, self->lights.array[i]);
	}

	/* Set state. */
	self->realized = value;

	return 1;
}

/*****************************************************************************/

static void
private_build_bounds (LIRenGroup*       self,
                      LIRenGroupObject* object,
                      int               first)
{
	LIMatAabb aabb;
	LIMatMatrix matrix;

	matrix = limat_convert_transform_to_matrix (object->transform);
	liren_model_get_bounds (object->model, &aabb);
	aabb = limat_aabb_transform (aabb, &matrix);
	object->aabb = aabb;
	if (first)
		self->aabb = aabb;
	else
		self->aabb = limat_aabb_union (self->aabb, aabb);
}

static void
private_build_lights (LIRenGroup*       self,
                      LIRenGroupObject* object)
{
	LIMatTransform tmp;
	LIMdlNode* node;
	LIMdlNodeIter iter;
	LIRenLight* light;

	LIMDL_FOREACH_NODE (iter, &object->model->model->nodes)
	{
		node = iter.value;
		if (node->type != LIMDL_NODE_LIGHT)
			continue;
		light = liren_light_new_from_model (self->scene, node);
		if (light == NULL)
			continue;
		if (!lialg_array_append (&self->lights, &light))
		{
			liren_light_free (light);
			continue;
		}
		tmp = light->transform;
		tmp = limat_transform_multiply (object->transform, tmp);
		liren_light_set_transform (light, &tmp);
		if (self->realized)
			liren_lighting_insert_light (self->scene->lighting, light);
	}
}

/** @} */
/** @} */
