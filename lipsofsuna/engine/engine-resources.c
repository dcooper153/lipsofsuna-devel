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
 * \addtogroup lieng Engine
 * @{
 * \addtogroup LIEngResources Resources
 * @{
 */

#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "engine-resources.h"

static LIEngModel*
private_load_model (LIEngResources* self,
                    const char*     name);

/*****************************************************************************/

/**
 * \brief Creates a new resource manager.
 *
 * The resource list is left empty after creation. The appropriate function
 * for loading the resources needs to be called manually.
 *
 * \param engine Engine.
 * \return Resource manager or NULL.
 */
LIEngResources*
lieng_resources_new (LIEngEngine* engine)
{
	LIEngResources* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIEngResources));
	if (self == NULL)
		return NULL;
	self->engine = engine;

	/* Allocate models. */
	self->models = lialg_strdic_new ();
	if (self->models == NULL)
	{
		lieng_resources_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the resource manager.
 *
 * \param self Resources.
 */
void
lieng_resources_free (LIEngResources* self)
{
	if (self->models != NULL)
	{
		lieng_resources_clear (self);
		lialg_strdic_free (self->models);
	}
	lisys_free (self);
}

/**
 * \brief Unloads all resources.
 *
 * \param self Resources.
 */
void
lieng_resources_clear (LIEngResources* self)
{
	LIAlgStrdicIter iter;

	if (self->models != NULL)
	{
		LIALG_STRDIC_FOREACH (iter, self->models)
			lieng_model_free (iter.value);
		lialg_strdic_clear (self->models);
	}
}

/**
 * \brief Finds a model by name.
 *
 * \param self Resources.
 * \param name Name.
 * \return Model or NULL.
 */
LIEngModel*
lieng_resources_find_model (LIEngResources* self,
                            const char*     name)
{
	LIEngModel* model;

	/* Try existing. */
	model = lialg_strdic_find (self->models, name);
	if (model != NULL)
		return model;

	return private_load_model (self, name);
}

/*****************************************************************************/

static LIEngModel*
private_load_model (LIEngResources* self,
                    const char*     name)
{
	LIEngModel* model;

	/* Create engine model. */
	model = lieng_model_new (self->engine, self->engine->config.dir, name);
	if (model == NULL)
		return NULL;

	/* Load model data. */
	if (!lieng_model_load (model))
	{
		lieng_model_free (model);
		return NULL;
	}

	/* Add to dictionary. */
	if (!lialg_strdic_insert (self->models, name, model))
	{
		lieng_model_free (model);
		return NULL;
	}

	return model;
}

/** @} */
/** @} */
