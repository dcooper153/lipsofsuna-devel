/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtObjectRender ObjectRender
 * @{
 */

#include "lipsofsuna/object.h"
#include "ext-module.h"

static int private_engine_free (
	LIExtModule* self,
	LIEngEngine* engine);

static int private_model_changed (
	LIExtModule* self,
	LIEngModel*  model);

static int private_model_free (
	LIExtModule* self,
	LIEngModel*  model);

static int private_model_new (
	LIExtModule* self,
	LIEngModel*  model);

static int private_object_new (
	LIExtModule* self,
	LIObjObject* object);

static int private_object_free (
	LIExtModule* self,
	LIObjObject* object);

static int private_object_model (
	LIExtModule* self,
	LIObjObject* object,
	LIEngModel*  model);

static int private_object_realize (
	LIExtModule* self,
	LIObjObject* object,
	int          value);

static int private_object_transform (
	LIExtModule*    self,
	LIObjObject*    object,
	LIMatTransform* value);

/*****************************************************************************/

LIMaiExtensionInfo liext_object_render_info =
{
	LIMAI_EXTENSION_VERSION, "ObjectRender",
	liext_object_render_new,
	liext_object_render_free
};

LIExtModule* liext_object_render_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Find the client compoent. */
	self->render = limai_program_find_component (program, "render");
	if (self->render == NULL)
	{
		liext_object_render_free (self);
		return NULL;
	}

	/* Make sure that the required extensions are loaded. */
	if (!limai_program_insert_extension (program, "render"))
	{
		liext_object_render_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, "engine-free", 1, private_engine_free, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, "model-changed", 1, private_model_changed, self, self->calls + 1) ||
	    !lical_callbacks_insert (program->callbacks, "model-free", 1, private_model_free, self, self->calls + 2) ||
	    !lical_callbacks_insert (program->callbacks, "model-new", 1, private_model_new, self, self->calls + 3) ||
	    !lical_callbacks_insert (program->callbacks, "object-new", 1, private_object_new, self, self->calls + 4) ||
	    !lical_callbacks_insert (program->callbacks, "object-free", 1, private_object_free, self, self->calls + 5) ||
	    !lical_callbacks_insert (program->callbacks, "object-model", 1, private_object_model, self, self->calls + 6) ||
	    !lical_callbacks_insert (program->callbacks, "object-visibility", 1, private_object_realize, self, self->calls + 7) ||
	    !lical_callbacks_insert (program->callbacks, "object-transform", 1, private_object_transform, self, self->calls + 8))
	{
		liext_object_render_free (self);
		return NULL;
	}

	/* Extend classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_RENDER_OBJECT, self);
	liext_script_render_object (program->script);

	return self;
}

void liext_object_render_free (
	LIExtModule* self)
{
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

/*****************************************************************************/

static int private_engine_free (
	LIExtModule* self,
	LIEngEngine* engine)
{
	return 1;
}

static int private_model_changed (
	LIExtModule* self,
	LIEngModel*  model)
{
	liren_render_model_set_model (self->render, model->id, model->model);

	return 1;
}

static int private_model_free (
	LIExtModule* self,
	LIEngModel*  model)
{
	lisys_assert (model != NULL);

	liren_render_model_free (self->render, model->id);

	return 1;
}

static int private_model_new (
	LIExtModule* self,
	LIEngModel*  model)
{
	liren_render_model_new (self->render, model->model, model->id);

	return 1;
}

static int private_object_new (
	LIExtModule* self,
	LIObjObject* object)
{
	liren_render_object_new (self->render, object->id);

	return 1;
}

static int private_object_free (
	LIExtModule* self,
	LIObjObject* object)
{
	liren_render_object_free (self->render, object->id);

	return 1;
}

static int private_object_model (
	LIExtModule* self,
	LIObjObject* object,
	LIEngModel*  model)
{
	if (model != NULL)
		liren_render_object_set_model (self->render, object->id, model->id);
	else
		liren_render_object_set_model (self->render, object->id, 0);

	return 1;
}

static int private_object_realize (
	LIExtModule* self,
	LIObjObject* object,
	int          value)
{
	liren_render_object_set_realized (self->render, object->id, value);

	return 1;
}

static int private_object_transform (
	LIExtModule*    self,
	LIObjObject*    object,
	LIMatTransform* value)
{
	liren_render_object_set_transform (self->render, object->id, value);

	return 1;
}

/** @} */
/** @} */
