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

#ifndef __EXT_PARTICLE_PARTICLE_MODEL_SYSTEM_HPP__
#define __EXT_PARTICLE_PARTICLE_MODEL_SYSTEM_HPP__

#include <vector>
#include "model-particles.h"
#include "particle.hpp"
#include "particle-driver.hpp"
#include <OgreParticle.h>

class LIExtParticleModelSystem
{
public:
	LIExtParticleModelSystem (LIExtParticle* parent, LIExtModelParticles* data, int index);
	~LIExtParticleModelSystem ();
	void update(float secs);
	void set_emitting(bool value);
	void set_looping(bool value);
	void set_render_queue(int render_queue);
protected:
	int index;
	bool emitting;
	bool looping;
	float time;
	LIExtParticle* parent;
	LIExtModelParticles* data;
	LIExtModelParticleSystem* system;
	Ogre::ParticleSystem* particles;
	std::vector<Ogre::Particle*> created;
};

#endif
