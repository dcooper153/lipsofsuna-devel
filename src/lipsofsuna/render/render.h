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

#ifndef __RENDER_H__
#define __RENDER_H__

#include "lipsofsuna/paths.h"
#include "lipsofsuna/system.h"
#include "render-types.h"

LIAPICALL (LIRenRender*, liren_render_new, (
	LIPthPaths* paths));

LIAPICALL (void, liren_render_free, (
	LIRenRender* self));

LIAPICALL (int, liren_render_load_image, (
	LIRenRender* self,
	const char*  name));

LIAPICALL (void, liren_render_reload, (
	LIRenRender* self,
	int          pass));

LIAPICALL (void, liren_render_remove_model, (
	LIRenRender* self,
	int          id));

LIAPICALL (void, liren_render_render, (
	LIRenRender* self,
	int          width,
	int          height));

LIAPICALL (void, liren_render_update, (
	LIRenRender* self,
	float        secs));

LIAPICALL (int, liren_render_get_anisotropy, (
	const LIRenRender* self));

LIAPICALL (void, liren_render_set_anisotropy, (
	LIRenRender* self,
	int          value));

LIAPICALL (int, liren_render_get_image_size, (
	LIRenRender* self,
	const char*  name,
	int*         result));

#endif
