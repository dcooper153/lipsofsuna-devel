/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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

#ifndef __RENDER_MATERIAL_H__
#define __RENDER_MATERIAL_H__

#include <lipsofsuna/image.h>
#include "render-image.h"
#include "render-shader.h"
#include "render-types.h"

enum
{
	LIREN_MATERIAL_FLAG_BILLBOARD = 0x010000,
	LIREN_MATERIAL_FLAG_CULLFACE = 0x020000,
	LIREN_MATERIAL_FLAG_TRANSPARENCY = 0x040000,
};

struct _LIRenMaterial
{
	int flags;
	float parameters[4];
	float shininess;
	float diffuse[4];
	float specular[4];
	float strand_start;
	float strand_end;
	float strand_shape;
	LIRenShader* shader_deferred;
	LIRenShader* shader_forward;
	struct
	{
		int count;
		LIRenTexture* array;
	} textures;
};

LIAPICALL (LIRenMaterial*, liren_material_new, ());

LIAPICALL (LIRenMaterial*, liren_material_new_from_model, (
	LIRenRender*         render,
	const LIMdlMaterial* material));

LIAPICALL (void, liren_material_free, (
	LIRenMaterial* self));

LIAPICALL (void, liren_material_set_diffuse, (
	LIRenMaterial* self,
	const float*   value));

LIAPICALL (void, liren_material_set_flags, (
	LIRenMaterial* self,
	int            flags));

LIAPICALL (int, liren_material_set_shader, (
	LIRenMaterial* self,
	LIRenShader*   deferred,
	LIRenShader*   forward));

LIAPICALL (void, liren_material_set_specular, (
	LIRenMaterial* self,
	const float*   value));

LIAPICALL (void, liren_material_set_texture, (
	LIRenMaterial* self,
	int            index,
	LIMdlTexture*  texture,
	LIRenImage*    image));

LIAPICALL (int, liren_material_set_texture_count, (
	LIRenMaterial* self,
	int            value));

#endif
