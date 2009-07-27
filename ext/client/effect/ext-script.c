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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliEffect Effect
 * @{
 */

#include <client/lips-client.h>
#include <script/lips-script.h>
#include "ext-module.h"

/* @luadoc
 * module "Extension.Client.Effect"
 * ---
 * -- Create special effects.
 * -- @name Effect
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new particle.
 * --
 * -- @param self Effect class.
 * -- @param pos Position vector.
 * -- @param vel Velocity vector.
 * function Effect.particle(self, pos, vel)
 */
static int
Effect_particle (lua_State* lua)
{
	liextModule* module;
	liparPoint* point;
	liscrData* pos;
	liscrData* vel;
	liscrScript* script;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EFFECT);
	pos = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	vel = liscr_checkdata (lua, 3, LICOM_SCRIPT_VECTOR);

	point = lirnd_scene_insert_particle (module->module->engine->scene, pos->data, vel->data);
	if (point != NULL)
	{
		point->time_fade = 0.0f;
		point->time_life = 1.0f;
	}
	return 0;
}

/* @luadoc
 * ---
 * -- Creates a new ray effect.
 * --
 * -- @param self Effect class.
 * -- @param src Source position vector.
 * -- @param dst Destination position vector.
 * -- @param life Ray life time.
 * function Effect.ray(self, src, dst, life)
 */
static int
Effect_ray (lua_State* lua)
{
	float life;
	liextModule* module;
	limatVector tmp;
	liparLine* line;
	liscrData* src;
	liscrData* dst;
	liscrScript* script;

	script = liscr_script (lua);
	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EFFECT);
	src = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	dst = liscr_checkdata (lua, 3, LICOM_SCRIPT_VECTOR);
	life = luaL_checknumber (lua, 4);

	/* FIXME */
	tmp = limat_vector_init (0.0f, 0.0f, 0.0f);
	line = lipar_manager_insert_line (module->module->engine->scene->particles, src->data, dst->data, &tmp, &tmp);
	if (line != NULL)
	{
		line->time_fade[0] = line->time_fade[1] = 0.0f;
		line->time_life[0] = line->time_life[1] = life;
	}
	return 0;
}

/* @luadoc
 * ---
 * -- Creates a particle system.
 * --
 * -- @param self Effect class.
 * -- @param pos Position vector.
 * -- @param name Effect name.
 * function Effect.system(self, pos, name)
 */
static int
Effect_system (lua_State* lua)
{
	int i;
	const char* effect;
	liextModule* module;
	limatVector position;
	limatVector velocity;
	liparPoint* particle;
	liscrData* vector;

	module = liscr_checkclassdata (lua, 1, LIEXT_SCRIPT_EFFECT);
	vector = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	effect = luaL_checkstring (lua, 3);

#warning Effect_system is not implemented properly yet.
	for (i = 0 ; i < 100 ; i++)
	{
		position = *((limatVector*) vector->data);;
		velocity = limat_vector_init (
			0.1*(rand()/(0.5*RAND_MAX)-1.0),
			0.1*(rand()/(3.0*RAND_MAX)+3.0),
			0.1*(rand()/(0.5*RAND_MAX)-1.0));
		particle = lirnd_scene_insert_particle (module->module->engine->scene, &position, &velocity);
		if (particle != NULL)
		{
			particle->time_life = 2.0f;
			particle->time_fade = 1.0f;
			particle->acceleration = limat_vector_init (0.0, -10.0, 0.5);
		}
	}

	return 0;
}

/*****************************************************************************/

void
liextEffectScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_EFFECT, data);
	liscr_class_insert_func (self, "particle", Effect_particle);
	liscr_class_insert_func (self, "ray", Effect_ray);
	liscr_class_insert_func (self, "system", Effect_system);
}

/** @} */
/** @} */
/** @} */
