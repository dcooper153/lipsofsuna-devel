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

#ifndef __RENDER_OVERLAY_OVERLAY_MANAGER_HPP__
#define __RENDER_OVERLAY_OVERLAY_MANAGER_HPP__

#include "lipsofsuna/algorithm.h"
#include "lipsofsuna/render.h"
#include "render-overlay.hpp"

class LIRenContainerFactory;
class LIRenImageOverlayFactory;
class LIRenScaledOverlayFactory;
class LIRenTextOverlayFactory;

class LIRenOverlayManager
{
public:
	LIRenOverlayManager (LIRenRender* render);
	~LIRenOverlayManager ();

// FIXME
public:
	LIAlgU32dic* overlays;
	Ogre::OverlayManager* overlay_manager;
	LIRenContainerFactory* container_factory;
	LIRenImageOverlayFactory* image_factory;
	LIRenScaledOverlayFactory* scaled_factory;
	LIRenTextOverlayFactory* text_factory;
#if OGRE_VERSION_MAJOR >= 1 && OGRE_VERSION_MINOR >= 9
	Ogre::OverlaySystem* overlay_system;
#endif
};

#endif
