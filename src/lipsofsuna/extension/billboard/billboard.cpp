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
 * \addtogroup LIExtBillboard Billboard
 * @{
 */

#include "billboard.hpp"

LIExtBillboard::LIExtBillboard (LIRenRender* render) : render(render)
{
	render_queue = Ogre::RENDER_QUEUE_MAIN;
	billboards = new Ogre::BillboardSet (render->id.next ());
	billboards->setDefaultWidth (1.0f);
	billboards->setDefaultHeight (1.0f);
	node = render->scene_root->createChildSceneNode ();
	node->setVisible (false);
	node->attachObject (billboards);
}

LIExtBillboard::~LIExtBillboard ()
{
	if (node != NULL)
		render->scene_root->removeAndDestroyChild (node->getName ());
	delete billboards;
}

void LIExtBillboard::set_render_queue (
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
	billboards->setRenderQueueGroup (render_queue);
}

void liext_billboard_free (
	LIExtBillboard* self)
{
	delete self;
}

/** @} */
/** @} */
