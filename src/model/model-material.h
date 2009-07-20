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

typedef int limdlTextureFlags;
typedef int limdlTextureType;

enum
{
	LIMDL_TEXTURE_FLAG_BILINEAR = 0x01,
	LIMDL_TEXTURE_FLAG_CLAMP    = 0x02,
	LIMDL_TEXTURE_FLAG_MIPMAP   = 0x04,
	LIMDL_TEXTURE_FLAG_REPEAT   = 0x08
};

enum
{
	LIMDL_TEXTURE_TYPE_EMPTY,
	LIMDL_TEXTURE_TYPE_ENVMAP,
	LIMDL_TEXTURE_TYPE_IMAGE
};

/*****************************************************************************/

typedef struct _limdlTexture limdlTexture;
struct _limdlTexture
{
	limdlTextureType type;
	limdlTextureFlags flags;
	int width;
	int height;
	char* string;
};

static inline int
limdl_texture_compare (const limdlTexture* self,
                       const limdlTexture* texture)
{
	if (self->type != texture->type ||
	    self->flags != texture->flags ||
	    self->width != texture->width ||
	    self->height != texture->height)
		return 0;
	if (strcmp (self->string, texture->string))
		return 0;

	return 1;
}

/*****************************************************************************/

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

void
limdl_material_clear_textures (limdlMaterial* self);

int
limdl_material_read (limdlMaterial* self,
                     liReader*      reader);

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
