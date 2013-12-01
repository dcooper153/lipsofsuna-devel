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

#include "particle.hpp"
#include "particle-driver.hpp"
#include "particle-driver-ogre.hpp"
#include "particle-driver-model.hpp"

/**
 * \brief Initializes the particle effect.
 * \param render Renderer.
 */
LIExtParticle::LIExtParticle (LIRenRender* render) :
	render(render), emitting(true), looping(true)
{
	render_queue = Ogre::RENDER_QUEUE_MAIN;

	node = render->scene_root->createChildSceneNode ();
	node->setVisible (false);
}

/**
 * \brief Frees the particle effect.
 */
LIExtParticle::~LIExtParticle ()
{
	for (size_t i = 0 ; i < drivers.size() ; i++)
		delete drivers[i];
	render->scene_root->removeAndDestroyChild (node->getName ());
}

/**
 * \brief Adds a particle effect from a model.
 * \param model Model.
 */
void LIExtParticle::add_model (
	const LIMdlModel* model)
{
	LIExtParticleDriverModel* driver = new LIExtParticleDriverModel(this, model);
	driver->set_emitting(emitting);
	driver->set_looping(looping);
	driver->set_render_queue(render_queue);
	drivers.push_back(driver);
}

/**
 * \brief Adds a particle effect from an Ogre script.
 * \param name Script name.
 */
void LIExtParticle::add_ogre (
	const char* name)
{
	LIExtParticleDriverOgre* driver = new LIExtParticleDriverOgre(this, name);
	driver->set_emitting(emitting);
	driver->set_looping(looping);
	driver->set_render_queue(render_queue);
	drivers.push_back(driver);
}

/**
 * \brief Instantly removes all the particles.
 */
void LIExtParticle::clear ()
{
	for (size_t i = 0 ; i < drivers.size() ; i++)
		delete drivers[i];
	drivers.clear ();
}

/**
 * \brief Updates the particle effect.
 * \param secs Seconds since the last update.
 */
void LIExtParticle::update (
	float secs)
{
	for (size_t i = 0 ; i < drivers.size() ; i++)
		drivers[i]->update (secs);
}

/**
 * \brief Enables or disables particle emission.
 * \param value True to enable. False otherwise.
 */
void LIExtParticle::set_emitting (
	bool value)
{
	emitting = value;
	for (size_t i = 0 ; i < drivers.size() ; i++)
		drivers[i]->set_emitting (value);
}

/**
 * \brief Enables or disables looping.
 * \param value True to enable. False otherwise.
 */
void LIExtParticle::set_looping (
	bool value)
{
	looping = value;
	for (size_t i = 0 ; i < drivers.size() ; i++)
		drivers[i]->set_looping (value);
}

/**
 * \brief Sets the render queue of the particle effect.
 * \param value Queue name.
 */
void LIExtParticle::set_render_queue (
	const char* value)
{
	if (!strcmp (value, "background"))
		render_queue = Ogre::RENDER_QUEUE_BACKGROUND;
	else if (!strcmp (value, "skies_early"))
		render_queue = Ogre::RENDER_QUEUE_SKIES_EARLY;
	else if (!strcmp (value, "1"))
		render_queue = Ogre::RENDER_QUEUE_1;
	else if (!strcmp (value, "2"))
		render_queue = Ogre::RENDER_QUEUE_2;
	else if (!strcmp (value, "3"))
		render_queue = Ogre::RENDER_QUEUE_3;
	else if (!strcmp (value, "4"))
		render_queue = Ogre::RENDER_QUEUE_4;
	else if (!strcmp (value, "main"))
		render_queue = Ogre::RENDER_QUEUE_MAIN;
	else if (!strcmp (value, "6"))
		render_queue = Ogre::RENDER_QUEUE_6;
	else if (!strcmp (value, "7"))
		render_queue = Ogre::RENDER_QUEUE_7;
	else if (!strcmp (value, "8"))
		render_queue = Ogre::RENDER_QUEUE_8;
	else if (!strcmp (value, "9"))
		render_queue = Ogre::RENDER_QUEUE_9;
	else if (!strcmp (value, "skies_late"))
		render_queue = Ogre::RENDER_QUEUE_SKIES_LATE;
	else if (!strcmp (value, "overlay"))
		render_queue = Ogre::RENDER_QUEUE_OVERLAY;

	for (size_t i = 0 ; i < drivers.size() ; i++)
		drivers[i]->set_render_queue (render_queue);
}

/**
 * \brief Frees the particle effect.
 * \param self Particle effect.
 */
void liext_particle_free (
	LIExtParticle* self)
{
	delete self;
}

/** @} */
/** @} */
