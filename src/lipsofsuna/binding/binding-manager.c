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
 * \addtogroup libnd Binding
 * @{
 * \addtogroup LIBndManager Manager
 * @{
 */

#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "binding-manager.h"

/**
 * \brief Creates a new binding manager.
 *
 * \return New binding manager or NULL.
 */
LIBndManager*
libnd_manager_new ()
{
	LIBndManager* self;

	self = lisys_calloc (1, sizeof (LIBndManager));
	if (self == NULL)
		return NULL;
	return self;
}

/**
 * \brief Frees the binding manager.
 *
 * \param self Binding manager.
 */
void
libnd_manager_free (LIBndManager* self)
{
	LIBndAction* action;
	LIBndAction* action_next;
	LIBndBinding* binding;
	LIBndBinding* binding_next;

	for (action = self->actions ; action != NULL ; action = action_next)
	{
		action_next = action->next;
		libnd_action_free (action);
	}
	for (binding = self->bindings ; binding != NULL ; binding = binding_next)
	{
		binding_next = binding->next;
		libnd_binding_free (binding);
	}
	lisys_free (self);
}

/**
 * \brief Calls the best handler matching an input event.
 *
 * \param self Binding manager.
 * \param type Event type.
 * \param code Key symbol, mouse button, mouse axis, joystick button, or joystick axis.
 * \param mods Modifier mask.
 * \param value Value within range from zero to one, inclusive.
 * \return Nonzero if an action was executed.
 */
int
libnd_manager_event (LIBndManager* self,
                     LIBndType     type,
                     uint32_t      code,
                     uint32_t      mods,
                     float         value)
{
	LIBndBinding* binding;
	LIBndBinding* binding_best = NULL;

	for (binding = self->bindings ; binding != NULL ; binding = binding->next)
	{
		if (!binding->action->enabled || binding->type != type)
			continue;
		if (binding->code != code ||
		   (binding->mods & mods) != binding->mods)
			continue;
		if (binding_best != NULL &&
		    binding_best->priority > binding->priority)
			continue;
		binding_best = binding;
	}

	if (binding_best != NULL)
	{
		if (binding_best->action->callback (
			binding_best->action, binding_best, binding_best->multiplier * value,
			binding_best->action->data))
			return 1;
	}

	return 0;
}

/**
 * \brief Finds an action by identifier.
 *
 * \param self Binding manager.
 * \param id Action identifier.
 * \return Action owned by the manager or NULL.
 */
LIBndAction*
libnd_manager_find_action (LIBndManager* self,
                           const char*   id)
{
	LIBndAction* action;

	for (action = self->actions ; action != NULL ; action = action->next)
	{
		if (!strcmp (action->identifier, id))
			return action;
	}

	return NULL;
}

/** @} */
/** @} */
