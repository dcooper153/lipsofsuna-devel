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

#include "input.hpp"

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
	(void*) liinp_input_new,
	(void*) liinp_input_free
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
	self = (LIInpInput*) lisys_calloc (1, sizeof (LIInpInput));
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
	self->client = (LICliClient*) limai_program_find_component (program, "client");
	if (self->client == NULL)
	{
		liinp_input_free (self);
		return NULL;
	}
	self->render = self->client->render;

	/* Initialize the input system. */
	self->system = new LIInpSystem (self, false);

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
	lical_callbacks_insert (program->callbacks, "tick", -1000, (void*) private_tick, self, self->calls + 0);

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

	/* Uninitialize the input system. */
	if (self->system != NULL)
		delete self->system;

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
	self->system->get_mouse_state (x, y);
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
	self->grab = value;
	if (self->active && value)
		private_grab (self, 1);
	else
		private_grab (self, 0);
}

/*****************************************************************************/

static void private_grab (
	LIInpInput* self,
	int         value)
{
#ifdef ENABLE_GRABS
	if (value != self->system->grab)
	{
		delete self->system;
		self->system = new LIInpSystem (self, value);
	}
#endif
}

static int private_tick (
	LIInpInput* self,
	float       secs)
{
	self->system->update (secs);

	return 1;
}

/** @} */
/** @} */