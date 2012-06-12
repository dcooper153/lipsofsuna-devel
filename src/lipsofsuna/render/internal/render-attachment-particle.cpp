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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenAttachmentParticle AttachmentParticle
 * @{
 */

#include "lipsofsuna/system.h"
#include "render.h"
#include "render-attachment-particle.hpp"

LIRenAttachmentParticle::LIRenAttachmentParticle (LIRenObject* object, const Ogre::String& particle) :
	LIRenAttachment (object)
{
	particles = NULL;
	try
	{
		/* Attach a new particle system to the scene node. */
		Ogre::String e_name = render->data->id.next ();
		particles = render->data->scene_manager->createParticleSystem (e_name, particle);
		object->node->attachObject (particles);

		/* Set particle effect visibility. */
		particles->setVisible (object->visible);
	}
	catch (...)
	{
	}
}

LIRenAttachmentParticle::~LIRenAttachmentParticle ()
{
	if (particles != NULL)
		render->data->scene_manager->destroyParticleSystem (particles);
}

void LIRenAttachmentParticle::update (float secs)
{
}

void LIRenAttachmentParticle::set_emitting (bool value)
{
	if (particles != NULL)
		particles->setEmitting (value);
}

/** @} */
/** @} */
/** @} */
