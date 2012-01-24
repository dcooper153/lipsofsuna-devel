/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIExtGraphics Graphics
 * @{
 */

#include "ext-module.h"

static void private_initial_videomode (
	LIExtGraphics* self,
	int*           width,
	int*           height,
	int*           fullscreen,
	int*           vsync,
	int*           multisamples);

static int private_engine_tick (
	LIExtGraphics* self,
	float          secs);

/*****************************************************************************/

LIMaiExtensionInfo liext_graphics_info =
{
	LIMAI_EXTENSION_VERSION, "Graphics",
	liext_graphics_new,
	liext_graphics_free
};

LIExtGraphics* liext_graphics_new (
	LIMaiProgram* program)
{
	int width = 1024;
	int height = 768;
	int fullscreen = 0;
	int sync = 0;
	int multisamples = 0;
	LIExtGraphics* self;
	LIRenVideomode mode;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtGraphics));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Get the initial video mode. */
	private_initial_videomode (self, &width, &height, &fullscreen, &sync, &multisamples);

	/* Initialize the renderer. */
	mode.width = width;
	mode.height = height;
	mode.fullscreen = fullscreen;
	mode.sync = sync;
	mode.multisamples = multisamples;
	self->render = liren_render_new (self->program->paths, &mode);
	if (self->render == NULL)
	{
		liext_graphics_free (self);
		return NULL;
	}
	self->mode = mode;

	/* Register component. */
	if (!limai_program_insert_component (self->program, "graphics", self) ||
	    !limai_program_insert_component (self->program, "render", self->render))
	{
		liext_graphics_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, "tick", 1, private_engine_tick, self, self->calls + 0))
	{
		liext_graphics_free (self);
		return NULL;
	}

	/* Extend scripts. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_GRAPHICS, self);
	liext_script_graphics (program->script);

	return self;
}

void liext_graphics_free (
	LIExtGraphics* self)
{
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	/* Remove the components. */
	if (self->program != NULL)
	{
		limai_program_remove_component (self->program, "graphics");
		limai_program_remove_component (self->program, "render");
	}

	/* Free the graphics engine. */
	if (self->render != NULL)
		liren_render_free (self->render);

	lisys_free (self);
}

int liext_graphics_set_videomode (
	LIExtGraphics* self,
	int            width,
	int            height,
	int            fullscreen,
	int            sync)
{
	LIRenVideomode mode;

	mode.width = width;
	mode.height = height;
	mode.fullscreen = fullscreen;
	mode.sync = sync;
	if (!liren_render_set_videomode (self->render, &mode))
		return 0;
	self->mode = mode;

	return 1;
}

/*****************************************************************************/

static void private_initial_videomode (
	LIExtGraphics* self,
	int*           width,
	int*           height,
	int*           fullscreen,
	int*           vsync,
	int*           multisamples)
{
	lua_State* lua;

	/* Get the videomode table. */
	lua = liscr_script_get_lua (self->program->script);
	lua_getglobal (lua, "__initial_videomode");
	if (lua_type (lua, -1) != LUA_TTABLE)
	{
		lua_pop (lua, 1);
		return;
	}

	/* Get the width. */
	lua_pushnumber (lua, 1);
	lua_gettable (lua, -2);
	if (lua_type (lua, -1) == LUA_TNUMBER)
	{
		*width = (int) lua_tonumber (lua, -1);
		*width = LIMAT_CLAMP (*width, 32, 65536);
	}
	lua_pop (lua, 1);

	/* Get the height. */
	lua_pushnumber (lua, 2);
	lua_gettable (lua, -2);
	if (lua_type (lua, -1) == LUA_TNUMBER)
	{
		*height = (int) lua_tonumber (lua, -1);
		*height = LIMAT_CLAMP (*height, 32, 65536);
	}
	lua_pop (lua, 1);

	/* Get the fullscreen flag. */
	lua_pushnumber (lua, 3);
	lua_gettable (lua, -2);
	if (lua_type (lua, -1) == LUA_TBOOLEAN)
		*fullscreen = (int) lua_toboolean (lua, -1);
	lua_pop (lua, 1);

	/* Get the vsync flag. */
	lua_pushnumber (lua, 4);
	lua_gettable (lua, -2);
	if (lua_type (lua, -1) == LUA_TBOOLEAN)
		*vsync = (int) lua_toboolean (lua, -1);
	lua_pop (lua, 1);

	/* Get the number of multisamples. */
	lua_pushnumber (lua, 5);
	lua_gettable (lua, -2);
	if (lua_type (lua, -1) == LUA_TNUMBER)
	{
		*multisamples = (int) lua_tonumber (lua, -1);
		*multisamples = LIMAT_CLAMP (*multisamples, 0, 128);
	}
	lua_pop (lua, 1);

	/* Pop the videomode table. */
	lua_pop (lua, 1);
}

static int private_engine_tick (
	LIExtGraphics* self,
	float          secs)
{
	if (!liren_render_update (self->render, secs))
		self->program->quit = 1;

	return 1;
}

/** @} */
/** @} */
