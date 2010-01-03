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
 * \addtogroup LIBndAction Action
 * @{
 */

#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "binding.h"
#include "binding-manager.h"

/**
 * \brief Creates a new binding action.
 *
 * \param manager Binding manager.
 * \param identifier Internal name for the action.
 * \param name Short name for the action.
 * \param description Description of the action.
 * \param callback Function called when activated.
 * \param data User data passed to the callback.
 * \return New binding action or NULL.
 */
LIBndAction*
libnd_action_new (LIBndManager* manager,
                  const char*   identifier,
                  const char*   name,
                  const char*   description,
                  libndCallback callback,
                  void*         data)
{
	LIBndAction* self;

	self = lisys_calloc (1, sizeof (LIBndAction));
	if (self == NULL)
		return NULL;
	self->manager = manager;
	self->enabled = 1;
	self->identifier = listr_dup (identifier);
	self->name = listr_dup (name);
	self->description = listr_dup (description);
	self->callback = callback;
	self->data = data;
	if (self->identifier == NULL ||
	    self->name == NULL ||
	    self->description == NULL)
	{
		libnd_action_free (self);
		return NULL;
	}

	/* Add to manager. */
	if (manager->actions != NULL)
		manager->actions->prev = self;
	self->next = manager->actions;
	self->prev = NULL;
	manager->actions = self;

	return self;
}

/**
 * \brief Frees the binding action.
 *
 * \param self Binding action.
 */
void
libnd_action_free (LIBndAction* self)
{
	/* Remove from manager. */
	if (self->prev != NULL)
		self->prev->next = self->next;
	else
		self->manager->actions = self->next;
	if (self->next != NULL)
		self->next->prev = self->prev;

	/* Free self. */
	lisys_free (self->identifier);
	lisys_free (self->name);
	lisys_free (self->description);
	lisys_free (self);
}

int
libnd_action_get_enabled (LIBndAction* self)
{
	return self->enabled;
}

void
libnd_action_set_enabled (LIBndAction* self,
                          int          value)
{
	self->enabled = value;
}

void*
libnd_action_get_userdata (LIBndAction* self)
{
	return self->data;
}

void
libnd_action_set_userdata (LIBndAction* self,
                           void*        value)
{
	self->data = value;
}

/** @} */
/** @} */
