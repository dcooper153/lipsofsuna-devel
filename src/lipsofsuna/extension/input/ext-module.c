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

/**
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIInpInput Input
 * @{
 */

#include "ext-module.h"

#define ENABLE_GRABS

static void private_grab (
	LIInpInput* self,
	int         value);

static int private_tick (
	LIInpInput* self,
	float       secs);

/*****************************************************************************/

LIMaiExtensionInfo liext_input_info =
{
	LIMAI_EXTENSION_VERSION, "Input",
	liinp_input_new,
	liinp_input_free
};

/**
 * \brief Creates the input subsystem.
 * \param program Program.
 * \return Input subsystem or NULL.
 */
LIInpInput* liinp_input_new (
	LIMaiProgram* program)
{
	LIInpInput* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIInpInput));
	if (self == NULL)
		return NULL;
	self->active = 1;
	self->program = program;

	/* Make sure that the required extensions are loaded. */
	if (!limai_program_insert_extension (program, "render"))
	{
		liinp_input_free (self);
		return NULL;
	}
	self->client = limai_program_find_component (program, "client");
	if (self->client == NULL)
	{
		liinp_input_free (self);
		return NULL;
	}
	self->render = self->client->render;

	/* Initialize input. */
	self->joystick = SDL_JoystickOpen (0);
	SDL_EnableUNICODE (1);
	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

	/* Register the component. */
	if (!limai_program_insert_component (self->program, "input", self))
	{
		liinp_input_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIINP_SCRIPT_INPUT, self);
	liinp_script_input (program->script);

	/* Register callbacks. */
	lical_callbacks_insert (program->callbacks, "tick", -1000, private_tick, self, self->calls + 0);

	return self;
}

/**
 * \brief Frees the input subsystem.
 * \param self Input.
 */
void liinp_input_free (
	LIInpInput* self)
{
	/* Unregister callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	/* Remove the component. */
	if (self->program != NULL)
		limai_program_remove_component (self->program, "input");

	/* Uninitialize the joystick. */
	if (self->joystick != NULL)
		SDL_JoystickClose (self->joystick);

	lisys_free (self);
}

/**
 * \brief Gets the current pointer position.
 * \param self Input.
 * \param x Return location for the X coordinate.
 * \param y Return location for the Y coordinate.
 */
void liinp_input_get_pointer (
	LIInpInput* self,
	int*        x,
	int*        y)
{
	SDL_GetMouseState (x, y);
}

/**
 * \brief Returns nonzero if movement mode is active.
 * \param self Input.
 * \return Boolean.
 */
int liinp_input_get_pointer_grab (
	LIInpInput* self)
{
	return self->grab;
}

/**
 * \brief Enables or disables movement mode.
 *
 * When the movement mode is enabled, all mouse events are passed directly to
 * the scripts. Otherwise, the events are first passed to the user interface.
 *
 * \param self Input.
 * \param value Nonzero for movement mode, zero for user interface mode
 */
void liinp_input_set_pointer_grab (
	LIInpInput* self,
	int         value)
{
#ifdef ENABLE_GRABS
	self->grab = value;
	if (self->active && value)
		private_grab (self, 1);
	else
		private_grab (self, 0);
#else
	int cx;
	int cy;

	self->grab = value;
	if (value)
	{
		cx = self->window->mode.width / 2;
		cy = self->window->mode.height / 2;
		SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);
		SDL_WarpMouse (cx, cy);
		SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
	}
#endif
}

/*****************************************************************************/

static void private_grab (
	LIInpInput* self,
	int         value)
{
#ifdef ENABLE_GRABS
	if (value)
		SDL_WM_GrabInput (SDL_GRAB_ON);
	else
		SDL_WM_GrabInput (SDL_GRAB_OFF);
#endif
}

static int private_tick (
	LIInpInput* self,
	float       secs)
{
#ifndef ENABLE_GRABS
	int x;
	int y;
	int cx;
	int cy;
#endif
	char* str;
	SDL_Event event;

	/* Handle input events. */
	while (SDL_PollEvent (&event))
	{
		switch (event.type)
		{
			case SDL_JOYAXISMOTION:
				limai_program_event (self->program, "joystickmotion", "axis", LISCR_TYPE_INT, event.jaxis.axis + 1, "value", LISCR_TYPE_FLOAT, event.jaxis.value / 32768.0f, NULL);
				return 0;
			case SDL_JOYBUTTONDOWN:
			case SDL_JOYBUTTONUP:
				limai_program_event (self->program, (event.type == SDL_JOYBUTTONDOWN)? "joystickpress" : "joystickrelease", "button", LISCR_TYPE_INT, event.jbutton.button, NULL);
				return 0;
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				limai_program_event (self->program, (event.type == SDL_MOUSEBUTTONDOWN)? "mousepress" : "mouserelease", "button", LISCR_TYPE_INT, event.button.button, "x", LISCR_TYPE_INT, event.button.x, "y", LISCR_TYPE_INT, event.button.y, NULL);
				return 0;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_F4 &&
				   (event.key.keysym.mod & KMOD_ALT))
					self->program->quit = 1;
				if (event.key.keysym.sym == SDLK_F5 &&
				   (event.key.keysym.mod & KMOD_CTRL) &&
				   (event.key.keysym.mod & KMOD_SHIFT))
				{
					lical_callbacks_call (self->program->callbacks, "context-lost", lical_marshal_DATA_INT, 0);
					lical_callbacks_call (self->program->callbacks, "context-lost", lical_marshal_DATA_INT, 1);
				}
				/* Fall through */
			case SDL_KEYUP:
				str = NULL;
				if (event.key.keysym.unicode != 0)
					str = lisys_wchar_to_utf8 (event.key.keysym.unicode);
				if (str != NULL)
				{
					limai_program_event (self->program, (event.type == SDL_KEYDOWN)? "keypress" : "keyrelease", "code", LISCR_TYPE_INT, event.key.keysym.sym, "mods", LISCR_TYPE_INT, event.key.keysym.mod, "text", LISCR_TYPE_STRING, str, NULL);
					lisys_free (str);
				}
				else
				{
					limai_program_event (self->program, (event.type == SDL_KEYDOWN)? "keypress" : "keyrelease", "code", LISCR_TYPE_INT, event.key.keysym.sym, "mods", LISCR_TYPE_INT, event.key.keysym.mod, NULL);
				}
				return 0;
			case SDL_MOUSEMOTION:
				limai_program_event (self->program, "mousemotion", "x", LISCR_TYPE_INT, event.motion.x, "y", LISCR_TYPE_INT, event.motion.y, "dx", LISCR_TYPE_INT, event.motion.xrel, "dy", LISCR_TYPE_INT, event.motion.yrel, NULL);
				return 0;
			case SDL_QUIT:
				limai_program_event (self->program, "quit", NULL);
				break;
			case SDL_ACTIVEEVENT:
				if (event.active.state & SDL_APPINPUTFOCUS)
				{
					if (event.active.gain)
					{
						self->active = 1;
						self->program->sleep = 0;
						if (self->grab)
							private_grab (self, 1);
					}
					else
					{
						self->active = 0;
						self->program->sleep = 100000;
						if (self->grab)
							private_grab (self, 0);
					}
				}
				break;
			case SDL_VIDEORESIZE:
				self->mode.width = event.resize.w;
				self->mode.height = event.resize.h;
				break;
		}
	}

	/* Pointer warping in movement mode. */
#ifndef ENABLE_GRABS
	if (self->grab)
	{
		cx = self->mode.width / 2;
		cy = self->mode.height / 2;
		SDL_GetMouseState (&x, &y);
		if (x != cx || y != cy)
		{
			SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);
			SDL_WarpMouse (cx, cy);
			SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
		}
	}
#endif

	return 1;
}

/** @} */
/** @} */
