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

class LIRenModelData;
class LIRenRender;

class LIRenModel
{
public:
	LIRenModel (
		LIRenRender*      render,
		const LIMdlModel* model,
		int               id,
		bool              editable = false);

	~LIRenModel ();

	void replace_buffer_vtx_nml (
		const void* data);

	void replace_texture (
		const char* name,
		int         width,
		int         height,
		const void* pixels);

	LIRenModelData* get_editable () const;

	int get_id () const;

	int get_loaded () const;

	int set_model (
		const LIMdlModel* model);

private:

	void create_mesh (
		const LIMdlModel* model);

private:
	int id;
	bool editable;
	LIRenRender* render;

// FIXME
public:
	Ogre::MeshPtr mesh;
};

#endif
