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

#ifndef __RENDER_INTERNAL_ENTITY_HPP__
#define __RENDER_INTERNAL_ENTITY_HPP__

#include "lipsofsuna/system.h"
#include "lipsofsuna/model.h"
#include "render-entity-builder.hpp"
#include <OgreEntity.h>
#include <OgreResource.h>

class LIRenEntity : public Ogre::Entity
{
public:
	LIRenEntity (const Ogre::String& name, LIRenModel* model);
	virtual ~LIRenEntity ();
	void initialize ();
	void update_pose (float secs);
	void update_pose_buffer ();
	bool get_loaded () const;
	LIMdlModel* get_model () const;
	void set_pose (LIMdlPose* pose);
	LIMdlPose* get_pose ();
	LIRenModel* get_render_model ();
	LIRenEntity* get_replacing_entity ();
	void set_replacing_entity (LIRenEntity* entity);
public:
	virtual void _updateRenderQueue (Ogre::RenderQueue* queue);
protected:
	bool pose_changed;
	LIMdlPose* pose;
	LIRenEntity* replacing_entity;
	LIRenEntityBuilder builder;
	LIRenModel* model;
	Ogre::MeshPtr background_loaded_mesh;
};

#endif
