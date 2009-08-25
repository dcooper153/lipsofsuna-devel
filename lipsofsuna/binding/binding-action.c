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
 * \addtogroup libndAction Action
 * @{
 */

#include <string/lips-string.h>
#include <system/lips-system.h>
#include "binding.h"

/**
 * \brief Creates a new binding action.
 *
 * \param identifier Internal name for the action.
 * \param name Short name for the action.
 * \param description Description of the action.
 * \param callback Function called when activated.
 * \param data User data passed to the callback.
 * \return New binding action or NULL.
 */
libndAction*
libnd_action_new (const char*   identifier,
                  const char*   name,
                  const char*   description,
                  libndCallback callback,
                  void*         data)
{
	libndAction* self;

	self = lisys_calloc (1, sizeof (libndAction));
	if (self == NULL)
		return NULL;
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

	return self;
}

/**
 * \brief Frees the binding action.
 *
 * \param self Binding action.
 */
void
libnd_action_free (libndAction* self)
{
	lisys_free (self->identifier);
	lisys_free (self->name);
	lisys_free (self->description);
	lisys_free (self);
}

int
libnd_action_get_enabled (libndAction* self)
{
	return self->enabled;
}

void
libnd_action_set_enabled (libndAction* self,
                          int          value)
{
	self->enabled = value;
}

void*
libnd_action_get_userdata (libndAction* self)
{
	return self->data;
}

void
libnd_action_set_userdata (libndAction* self,
                           void*        value)
{
	self->data = value;
}

/** @} */
/** @} */
