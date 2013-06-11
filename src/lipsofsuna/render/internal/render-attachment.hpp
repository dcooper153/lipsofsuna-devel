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

#ifndef __RENDER_INTERNAL_ATTACHMENT_HPP__
#define __RENDER_INTERNAL_ATTACHMENT_HPP__

#include "lipsofsuna/system.h"
#include "lipsofsuna/model.h"
#include "render-types.h"

class LIRenAttachment
{
public:
	LIRenAttachment (LIRenObject* object);
	virtual ~LIRenAttachment ();
	virtual LIMdlNode* find_node (const char* name);
	virtual LIMdlModel* get_model () const;
	virtual bool has_model (LIRenModel* model);
	virtual bool is_loaded () const;
	virtual void remove_model (LIRenModel* model);
	virtual void replace_texture (const char* name, Ogre::TexturePtr& texture);
	virtual void update (float secs);
	virtual void update_pose (LIMdlPoseSkeleton* skeleton);
	virtual void update_settings ();
	virtual void set_emitting (bool value);
public:
	LIRenAttachment* get_replacer ();
	void set_replacer (LIRenAttachment* replacer);
protected:
	LIRenObject* object;
	LIRenRender* render;
	LIRenAttachment* replacer;
};

#endif
