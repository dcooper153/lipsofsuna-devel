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

#ifndef __RENDER_INTERNAL_MODEL_HPP__
#define __RENDER_INTERNAL_MODEL_HPP__

#include "lipsofsuna/model.h"
#include "lipsofsuna/system.h"
#include <OgreMesh.h>

class LIRenRender;

class LIRenModel
{
public:
	LIRenModel (
		LIRenRender* render,
		LIMdlModel*  model,
		int          id);

	~LIRenModel ();

	void replace_texture (
		const char* name,
		int         width,
		int         height,
		const void* pixels);

	int get_id () const;

	int get_loaded () const;

	LIMdlModel* get_model () const;

	int set_model (
		LIMdlModel* model);

private:

	void create_mesh (
		LIMdlModel* model);

private:
	int id;
	LIMdlModel* model;
	LIRenRender* render;

// FIXME
public:
	Ogre::MeshPtr mesh;
};

#endif
