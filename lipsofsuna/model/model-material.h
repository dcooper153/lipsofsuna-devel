/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup limdl Model
 * @{
 * \addtogroup limdlMaterial Material
 * @{
 */

#ifndef __MODEL_MATERIAL_H__
#define __MODEL_MATERIAL_H__

#include <string.h>
#include <archive/lips-archive.h>
#include "model-texture.h"

typedef struct _limdlMaterial limdlMaterial;
struct _limdlMaterial
{
	int flags;
	char* shader;
	float emission;
	float shininess;
	float diffuse[4];
	float specular[4];
	float strand_start;
	float strand_end;
	float strand_shape;
	struct
	{
		int count;
		limdlTexture* array;
	} textures;
};

#ifdef __cplusplus
extern "C" {
#endif

int
limdl_material_init (limdlMaterial* self);

int
limdl_material_init_copy (limdlMaterial*       self,
                          const limdlMaterial* src);

void
limdl_material_clear_textures (limdlMaterial* self);

int
limdl_material_read (limdlMaterial* self,
                     liarcReader*   reader);

int
limdl_material_realloc_textures (limdlMaterial* self,
                                 int            count);

int
limdl_material_write (limdlMaterial* self,
                      liarcWriter*   writer);

int
limdl_material_set_texture (limdlMaterial* self,
                            int            unit,
                            int            type,
                            int            flags,
                            const char*    name);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** @} */
