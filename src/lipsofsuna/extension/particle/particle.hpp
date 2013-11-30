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

#ifndef __EXT_PARTICLE_PARTICLE_HPP__
#define __EXT_PARTICLE_PARTICLE_HPP__

#include "lipsofsuna/extension.h"
#include "lipsofsuna/render/internal/render.hpp"

class LIExtParticleDriver;

class LIExtParticle
{
public:
	LIExtParticle (LIRenRender* render);
	~LIExtParticle ();
	void add_model (const LIMdlModel* model);
	void add_ogre (const char* name);
	void clear ();
	void set_emitting (bool value);
	void set_render_queue (const char* value);
public:
	LIRenRender* render;
	int render_queue;
	bool emitting;
	Ogre::SceneNode* node;
	std::vector<LIExtParticleDriver*> drivers;
};

LIAPICALL (void, liext_particle_free, (
	LIExtParticle* self));

#endif
