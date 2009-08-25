/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup libndManager Manager
 * @{
 */

#include <string/lips-string.h>
#include <system/lips-system.h>
#include "binding-manager.h"

/**
 * \brief Creates a new binding manager.
 *
 * \return New binding manager or NULL.
 */
libndManager*
libnd_manager_new ()
{
	libndManager* self;

	self = lisys_calloc (1, sizeof (libndManager));
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
libnd_manager_free (libndManager* self)
{
	libndAction* action;
	libndAction* action_next;
	libndBinding* binding;
	libndBinding* binding_next;

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
libnd_manager_event (libndManager* self,
                     libndType     type,
                     uint32_t      code,
                     uint32_t      mods,
                     float         value)
{
	libndBinding* binding;
	libndBinding* binding_best = NULL;

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
libndAction*
libnd_manager_find_action (libndManager* self,
                           const char*   id)
{
	libndAction* action;

	for (action = self->actions ; action != NULL ; action = action->next)
	{
		if (!strcmp (action->identifier, id))
			return action;
	}

	return NULL;
}

/**
 * \brief Adds a action to the manager.
 *
 * \param self Binding manager.
 * \param action Action to insert.
 */
void
libnd_manager_insert_action (libndManager* self,
                             libndAction*  action)
{
	assert (action != NULL);

	if (self->actions != NULL)
		self->actions->prev = action;
	action->next = self->actions;
	action->prev = NULL;
	self->actions = action;
}

/**
 * \brief Removes an action from the manager.
 *
 * The action must be freed manually after calling this.
 *
 * \param self Binding manager.
 * \param action Action to remove.
 */
void
libnd_manager_remove_action (libndManager* self,
                             libndAction*  action)
{
	assert (action != NULL);

	if (action->prev != NULL)
		action->prev->next = action->next;
	else
		self->actions = action->next;
	if (action->next != NULL)
		action->next->prev = action->prev;
	action->next = NULL;
	action->prev = NULL;
}

/**
 * \brief Adds a binding to the manager.
 *
 * \param self Binding manager.
 * \param binding Binding to insert.
 */
void
libnd_manager_insert_binding (libndManager* self,
                              libndBinding* binding)
{
	assert (binding != NULL);
	assert (binding->action != NULL);

	if (self->bindings != NULL)
		self->bindings->prev = binding;
	binding->next = self->bindings;
	binding->prev = NULL;
	self->bindings = binding;
}

/**
 * \brief Removes a binding from the manager.
 *
 * The binding must be freed manually after calling this.
 *
 * \param self Binding manager.
 * \param binding Binding to remove.
 */
void
libnd_manager_remove_binding (libndManager* self,
                              libndBinding* binding)
{
	assert (binding != NULL);
	assert (binding->action != NULL);

	if (binding->prev != NULL)
		binding->prev->next = binding->next;
	else
		self->bindings = binding->next;
	if (binding->next != NULL)
		binding->next->prev = binding->prev;
	binding->next = NULL;
	binding->prev = NULL;
}

/** @} */
/** @} */
