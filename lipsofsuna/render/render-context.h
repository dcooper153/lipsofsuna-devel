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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndContext Context
 * @{
 */

#ifndef __RENDER_CONTEXT_H__
#define __RENDER_CONTEXT_H__

#include <image/lips-image.h>
#include <model/lips-model.h>
#include "render-buffer.h"
#include "render-light.h"
#include "render-material.h"
#include "render-scene.h"
#include "render-texture.h"
#include "render-types.h"

struct _lirndContext
{
	int compiled;
	int fixed;
	int shadows;
	lirndRender* render;
	lirndScene* scene;
	lirndShader* shader;
	limatFrustum frustum;
	limatMatrix matrix;
	limatMatrix modelview;
	limatMatrix modelviewinverse;
	limatMatrix projection;
	struct
	{
		int count;
		lirndLight** array;
	} lights;
	struct
	{
		int flags;
		float parameters[4];
		float shininess;
		float diffuse[4];
		float specular[4];
	} material;
	struct
	{
		int count;
		lirndTexture* array;
	} textures;
};

void
lirnd_context_init (lirndContext* self,
                    lirndScene*   scene);

void
lirnd_context_bind (lirndContext* self);

void
lirnd_context_render_array (lirndContext* self,
                            lirndBuffer*  vertex);

void
lirnd_context_render_indexed (lirndContext* self,
                              lirndBuffer*  vertex,
                              lirndBuffer*  index);

void
lirnd_context_render (lirndContext* self,
                      lirndBuffer*  buffer);

void
lirnd_context_render_vbo_array (lirndContext*      self,
                                const lirndFormat* format,
                                GLuint             vertices,
                                int                vertex0,
                                int                vertex1);

void
lirnd_context_render_vtx_array (lirndContext*      self,
                                const lirndFormat* format,
                                const void*        vertices,
                                int                vertex0,
                                int                vertex1);

void
lirnd_context_render_vbo_indexed (lirndContext*      self,
                                  const lirndFormat* format,
                                  GLuint             vertices,
                                  GLuint             indices,
                                  int                index0,
                                  int                index1);

void
lirnd_context_render_vtx_indexed (lirndContext*      self,
                                  const lirndFormat* format,
                                  const void*        vertices,
                                  const void*        indices,
                                  int                index0,
                                  int                index1);

void
lirnd_context_unbind (lirndContext* self);

void
lirnd_context_set_flags (lirndContext* self,
                         int           value);

void
lirnd_context_set_frustum (lirndContext*       self,
                           const limatFrustum* frustum);

void
lirnd_context_set_lights (lirndContext* self,
                          lirndLight**  value,
                          int           count);

void
lirnd_context_set_material (lirndContext*        self,
                            const lirndMaterial* value);

void
lirnd_context_set_matrix (lirndContext*      self,
                          const limatMatrix* value);

void
lirnd_context_set_modelview (lirndContext*      self,
                             const limatMatrix* value);

void
lirnd_context_set_projection (lirndContext*      self,
                              const limatMatrix* value);

void
lirnd_context_set_shader (lirndContext* self,
                          lirndShader*  value);

void
lirnd_context_set_textures (lirndContext* self,
                            lirndTexture* value,
                            int           count);

#endif

/** @} */
/** @} */
