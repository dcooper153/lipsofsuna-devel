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

#include "particle-driver-model.hpp"

/**
 * \brief Initializes the driver.
 * \param parent Particle effect.
 * \param model Model.
 */
LIExtParticleDriverModel::LIExtParticleDriverModel(LIExtParticle* parent, const LIMdlModel* model) : LIExtParticleDriver(parent)
{
	liext_model_particles_init (&data, model);
	for (int i = 0 ; i < data.systems.count ; i++)
		systems.push_back(new LIExtParticleModelSystem (parent, &data, i));
}

/**
 * \brief Frees the driver.
 */
LIExtParticleDriverModel::~LIExtParticleDriverModel()
{
	for (size_t i = 0 ; i < systems.size() ; i++)
		delete systems[i];
	liext_model_particles_clear (&data);
}

/**
 * \brief Updates the driver.
 * \param secs Seconds since the last update.
 */
void LIExtParticleDriverModel::update (float secs)
{
	for (size_t i = 0 ; i < systems.size() ; i++)
		systems[i]->update (secs);
}

/**
 * \brief Enables or disables particle emission.
 * \param value True to enable. False otherwise.
 */
void LIExtParticleDriverModel::set_emitting (bool value)
{
	for (size_t i = 0 ; i < systems.size() ; i++)
		systems[i]->set_emitting (value);
}

/**
 * \brief Enables or disables looping.
 * \param value True to enable. False otherwise.
 */
void LIExtParticleDriverModel::set_looping (bool value)
{
	for (size_t i = 0 ; i < systems.size() ; i++)
		systems[i]->set_looping (value);
}

/**
 * \brief Sets the render queue of the particle effect.
 * \param render_queue Queue number.
 */
void LIExtParticleDriverModel::set_render_queue(int render_queue)
{
	for (size_t i = 0 ; i < systems.size() ; i++)
		systems[i]->set_render_queue (render_queue);
}

/** @} */
/** @} */
