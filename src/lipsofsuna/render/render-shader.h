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

#ifndef __RENDER_SHADER_H__
#define __RENDER_SHADER_H__

#include "lipsofsuna/system.h"
#include "render-types.h"

#define LIREN_SHADER_PASS_COUNT 10

LIAPICALL (int, liren_render_shader_new, (
	LIRenRender* render,
	const char*  name));

LIAPICALL (void, liren_render_shader_free, (
	LIRenRender* self,
	const char*  shader));

LIAPICALL (void, liren_render_shader_clear_pass, (
	LIRenRender* self,
	const char*  shader,
	int          pass));

LIAPICALL (int, liren_render_shader_compile, (
	LIRenRender* self,
	const char*  shader,
	int          pass,
	const char*  vertex,
	const char*  geometry,
	const char*  fragment,
	int          animated,
	int          alpha_to_coverage,
	int          blend_enable,
	int          blend_src,
	int          blend_dst,
	int          color_write,
	int          depth_test,
	int          depth_write,
	int          depth_func));

LIAPICALL (void, liren_render_shader_set_sort, (
	LIRenRender* self,
	const char*  shader,
	int          value));

#endif
