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

/**
 * \addtogroup limdl Model
 * @{
 * \addtogroup LIMdlMaterial Material
 * @{
 */

#ifndef __MODEL_MATERIAL_H__
#define __MODEL_MATERIAL_H__

#include <string.h>
#include <lipsofsuna/archive.h>
#include "model-texture.h"

typedef struct _LIMdlMaterial LIMdlMaterial;
struct _LIMdlMaterial
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
		LIMdlTexture* array;
	} textures;
};

#ifdef __cplusplus
extern "C" {
#endif

int
limdl_material_init (LIMdlMaterial* self);

int
limdl_material_init_copy (LIMdlMaterial*       self,
                          const LIMdlMaterial* src);

void
limdl_material_clear_textures (LIMdlMaterial* self);

int
limdl_material_compare (const LIMdlMaterial* self,
                        const LIMdlMaterial* material);

int
limdl_material_read (LIMdlMaterial* self,
                     LIArcReader*   reader);

int
limdl_material_realloc_textures (LIMdlMaterial* self,
                                 int            count);

int
limdl_material_write (LIMdlMaterial* self,
                      LIArcWriter*   writer);

int
limdl_material_set_texture (LIMdlMaterial* self,
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
