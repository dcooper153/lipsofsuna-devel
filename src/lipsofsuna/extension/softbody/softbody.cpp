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
 * \addtogroup LIExtSoftbody Softbody
 * @{
 */

#include "lipsofsuna/render/internal/render-model-data.hpp"
#include "softbody.hpp"

LIExtSoftbody::LIExtSoftbody (LIRenRender* render, const LIMdlModel* model) : render(render)
{
	this->model = new LIRenModel (render, model, 0, true);
	object = new LIRenObject (render, 0);
	object->add_model (this->model);
}

LIExtSoftbody::~LIExtSoftbody ()
{
	delete object;
	delete model;
}

void LIExtSoftbody::update (float secs)
{
	// TODO
	LIRenModelData* data = model->get_editable ();
	if (data == NULL || data->buffer_data_0 == NULL)
		return;
	for (size_t i = 0 ; i < data->vertex_count ; i++)
	{
		float* coord = data->buffer_data_0 + 6 * i;
		coord[0] += 0.1f * secs;
	}
	model->replace_buffer_vtx_nml (data->buffer_data_0);
}

void LIExtSoftbody::set_position (float x, float y, float z)
{
	object->set_position (x, y, z);
}

void LIExtSoftbody::set_rotation (float x, float y, float z, float w)
{
	object->set_rotation (x, y, z, w);
}

void LIExtSoftbody::set_render_queue (const char* value)
{
	object->set_render_queue (value);
}

void LIExtSoftbody::set_visible (int value)
{
	object->set_visible (value);
}

void liext_softbody_free (
	LIExtSoftbody* self)
{
	delete self;
}

/** @} */
/** @} */
