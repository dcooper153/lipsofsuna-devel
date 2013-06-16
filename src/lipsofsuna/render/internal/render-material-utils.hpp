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

#ifndef __RENDER_MATERIAL_UTILS_HPP__
#define __RENDER_MATERIAL_UTILS_HPP__

#include "lipsofsuna/system.h"
#include "lipsofsuna/model.h"
#include "render-types.hpp"
#include <OgreResource.h>

class LIRenMaterialUtils
{
public:
	LIRenMaterialUtils (LIRenRender* render);
public:
	Ogre::MaterialPtr create_material (LIMdlMaterial* mat);
	bool has_overridable_texture (const Ogre::MaterialPtr& material, const Ogre::String& name);
	void replace_texture (Ogre::MaterialPtr& material, const Ogre::String& name, const Ogre::String& new_name);
public:
	bool check_material_override (Ogre::MaterialPtr& material);
	bool check_name_override (const Ogre::String& name);
	void initialize_pass (LIMdlMaterial* mat, Ogre::Pass* pass);
	void override_pass (LIMdlMaterial* mat, Ogre::Pass* pass);
	Ogre::String texture_name_to_filename(const Ogre::String& name);
	Ogre::String texture_filename_to_name(const Ogre::String& filename);
private:
	LIRenRender* render;
};

#endif
