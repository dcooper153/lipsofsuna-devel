/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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

#ifndef __RENDER_INTERNAL_INTERNAL_H__
#define __RENDER_INTERNAL_INTERNAL_H__

#include "SDL.h"
#include "SDL_main.h"
#include "SDL_ttf.h"
#include <GL/glew.h>

#include "render.h"
#include "render-buffer.h"
#include "render-framebuffer.h"
#include "render-image.h"
#include "render-light.h"
#include "render-model.h"
#include "render-object.h"
#include "render-overlay.h"
#include "render-shader.h"
#include "render-types.h"

#if defined LIMDL_VERTEX_WEIGHT_UINT8
#define LIREN_WEIGHT_FORMAT GL_UNSIGNED_BYTE
#elif defined LIMDL_VERTEX_WEIGHT_UINT16
#define LIREN_WEIGHT_FORMAT GL_UNSIGNED_SHORT
#else
#define LIREN_WEIGHT_FORMAT GL_FLOAT
#endif

#endif
