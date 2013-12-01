/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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
 * \addtogroup LIExtParticle Particle
 * @{
 */

#include "module.h"
#include "particle.hpp"

static void Particle_new (LIScrArgs* args)
{
	LIExtParticle* self;
	LIExtParticleModule* module;
	LIScrData* data;

	/* Allocate self. */
	module = (LIExtParticleModule*) liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PARTICLE);
	self = new LIExtParticle (module->render);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_PARTICLE, (LIScrGCFunc) liext_particle_free);
	if (data == NULL)
	{
		liext_particle_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Particle_add_model (LIScrArgs* args)
{
	LIExtParticle* self = (LIExtParticle*) args->self;
	LIScrData* data;

	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_MODEL, &data))
		return;
	self->add_model((const LIMdlModel*) liscr_data_get_data (data));
}

static void Particle_add_ogre (LIScrArgs* args)
{
	const char* name;
	LIExtParticle* self = (LIExtParticle*) args->self;

	if (!liscr_args_geti_string (args, 0, &name))
		return;
	self->add_ogre(name);
}

static void Particle_clear (LIScrArgs* args)
{
	LIExtParticle* self = (LIExtParticle*) args->self;

	self->clear ();
}

static void Particle_update (LIScrArgs* args)
{
	float v;
	LIExtParticle* self = (LIExtParticle*) args->self;

	if (!liscr_args_geti_float (args, 0, &v))
		return;
	self->update (v);
}

static void Particle_set_emitting (LIScrArgs* args)
{
	int v;
	LIExtParticle* self = (LIExtParticle*) args->self;

	if (!liscr_args_geti_bool (args, 0, &v))
		return;
	self->set_emitting (v);
}

static void Particle_set_looping (LIScrArgs* args)
{
	int v;
	LIExtParticle* self = (LIExtParticle*) args->self;

	if (!liscr_args_geti_bool (args, 0, &v))
		return;
	self->set_looping (v);
}

static void Particle_set_position (LIScrArgs* args)
{
	float x;
	float y;
	float z;
	LIExtParticle* self = (LIExtParticle*) args->self;

	if (!liscr_args_geti_float (args, 0, &x) ||
	    !liscr_args_geti_float (args, 1, &y) ||
	    !liscr_args_geti_float (args, 2, &z))
		return;
	self->node->setPosition (x, y, z);
}

static void Particle_set_render_queue (LIScrArgs* args)
{
	const char* v;
	LIExtParticle* self = (LIExtParticle*) args->self;

	if (!liscr_args_geti_string (args, 0, &v))
		return;
	self->set_render_queue (v);
}

static void Particle_set_rotation (LIScrArgs* args)
{
	float x;
	float y;
	float z;
	float w;
	LIExtParticle* self = (LIExtParticle*) args->self;

	if (!liscr_args_geti_float (args, 0, &x) ||
	    !liscr_args_geti_float (args, 1, &y) ||
	    !liscr_args_geti_float (args, 2, &z) ||
	    !liscr_args_geti_float (args, 3, &w))
		return;
	self->node->setOrientation (w, x, y, z);
}

static void Particle_set_visible (LIScrArgs* args)
{
	int v;
	LIExtParticle* self = (LIExtParticle*) args->self;

	if (!liscr_args_geti_bool (args, 0, &v))
		return;
	self->node->setVisible (v);
}

/*****************************************************************************/

void liext_script_particle (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_PARTICLE, "particle_new", Particle_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PARTICLE, "particle_add_model", Particle_add_model);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PARTICLE, "particle_add_ogre", Particle_add_ogre);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PARTICLE, "particle_clear", Particle_clear);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PARTICLE, "particle_update", Particle_update);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PARTICLE, "particle_set_emitting", Particle_set_emitting);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PARTICLE, "particle_set_looping", Particle_set_looping);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PARTICLE, "particle_set_position", Particle_set_position);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PARTICLE, "particle_set_render_queue", Particle_set_render_queue);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PARTICLE, "particle_set_rotation", Particle_set_rotation);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PARTICLE, "particle_set_visible", Particle_set_visible);
}

/** @} */
/** @} */
