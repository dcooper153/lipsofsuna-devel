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

#include "particle-driver.hpp"

/**
 * \brief Initializes the driver.
 * \param parent Particle effect.
 */
LIExtParticleDriver::LIExtParticleDriver(LIExtParticle* parent) : parent(parent), particles(NULL)
{
}

/**
 * \brief Frees the driver.
 */
LIExtParticleDriver::~LIExtParticleDriver()
{
	if (particles)
	{
		parent->node->detachObject (particles);
		parent->render->scene_manager->destroyParticleSystem (particles);
	}
}

/**
 * \brief Enables or disables particle emission.
 * \param value True to enable. False otherwise.
 */
void LIExtParticleDriver::set_emitting (bool value)
{
	if (particles)
		particles->setEmitting (value);
}

/**
 * \brief Sets the render queue of the particle effect.
 * \param render_queue Queue number.
 */
void LIExtParticleDriver::set_render_queue(int render_queue)
{
	if (particles)
		particles->setRenderQueueGroup (render_queue);
}

/** @} */
/** @} */
