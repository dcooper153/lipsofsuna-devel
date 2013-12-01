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

#include "lipsofsuna/render/internal/render-material-utils.hpp"
#include "particle-model-system.hpp"

/**
 * \brief Initializes the particle subsystem.
 */
LIExtParticleModelSystem::LIExtParticleModelSystem (LIExtParticle* parent, LIExtModelParticles* data, int index) :
	index(index), emitting(true), looping(true), time(0.0f), parent(parent), data(data)
{
	// Get the particle system data.
	system = data->systems.array + index;
	int count = system->particle_end - system->particle_start;
	float size = 2.0f * system->particle_size;

	// Allocate the particle system.
	particles = parent->render->scene_manager->createParticleSystem (parent->render->id.next (), count);
	particles->setDefaultDimensions (size, size);
	particles->setKeepParticlesInLocalSpace (true);
	particles->setCastShadows (false);

	// Instantiate the material.
	Ogre::MaterialPtr instanced = parent->render->material_utils->create_instanced_material (system->material, system->texture);
	if (!instanced.isNull())
		particles->setMaterialName (instanced->getName ());
	else
		particles->setMaterialName (system->material);

	// Attach to the scene node.
	parent->node->attachObject (particles);
	created.resize(count, NULL);
}

/**
 * \brief Frees the particle subsystem.
 */
LIExtParticleModelSystem::~LIExtParticleModelSystem ()
{
	parent->node->detachObject (particles);
	parent->render->scene_manager->destroyParticleSystem (particles);
}

/**
 * \brief Updates the particle subsystem.
 * \param secs Seconds since the last update.
 */
void LIExtParticleModelSystem::update (float secs)
{
	float color[4];
	LIMatVector pos;

	time += secs;
	for (size_t i = 0 ; i < created.size() ; i++)
	{
		if (liext_model_particles_evaluate_particle (data, index, i, time, looping, &pos, color))
		{
			if (!created[i] && emitting)
				created[i] = particles->createParticle ();
			if (!created[i])
				continue;
			Ogre::Particle* particle = created[i];
			particle->mOwnDimensions = false;
			particle->mWidth = 2.0f * system->particle_size;
			particle->mHeight = 2.0f * system->particle_size;
			particle->rotation = 0.0f;
			particle->position = Ogre::Vector3 (pos.x, -pos.y, pos.z);
			particle->direction = Ogre::Vector3 (0.0f, 0.0f, 0.0f);
			particle->colour = Ogre::ColourValue (color[0], color[1], color[2], color[3]);
			particle->timeToLive = 1000.0f;
			particle->totalTimeToLive = 1000.0f;
			particle->rotationSpeed = 0.0f;
			particle->particleType = Ogre::Particle::Visual;
		}
		else if (created[i])
		{
			Ogre::Particle* particle = created[i];
			particle->timeToLive = 0.0f;
			particle->totalTimeToLive = 0.0f;
			created[i] = NULL;
		}
	}
}

/**
 * \brief Enables or disables particle emission.
 * \param value True to enable. False otherwise.
 */
void LIExtParticleModelSystem::set_emitting (bool value)
{
	emitting = value;
}

/**
 * \brief Enables or disables looping.
 * \param value True to enable. False otherwise.
 */
void LIExtParticleModelSystem::set_looping (bool value)
{
	looping = value;
}

/**
 * \brief Sets the render queue of the particle subsystem.
 * \param render_queue Queue number.
 */
void LIExtParticleModelSystem::set_render_queue(int render_queue)
{
	particles->setRenderQueueGroup (render_queue);
}

/** @} */
/** @} */
