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
 * \addtogroup LIBndBinding Binding
 * @{
 */

#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "binding.h"
#include "binding-manager.h"

/**
 * \brief Creates a new binding.
 *
 * \param manager Binding manager.
 * \param type Binding type.
 * \param action Action of the binding.
 * \param params Parameters to be passed to the action.
 * \param code Key symbol, mouse button, mouse axis, joystick button, or joystick axis.
 * \param mods Modifier mask.
 * \param multiplier Value of handled events will be premultiplied by this.
 * \return New binding or NULL.
 */
LIBndBinding*
libnd_binding_new (LIBndManager* manager,
                   LIBndType     type,
                   LIBndAction*  action,
                   const char*   params,
                   uint32_t      code,
                   uint32_t      mods,
                   float         multiplier)
{
	uint32_t mask;
	LIBndBinding* self;

	lisys_assert (action != NULL);

	self = lisys_calloc (1, sizeof (LIBndBinding));
	if (self == NULL)
		return NULL;
	if (params == NULL)
		params = "";
	self->params = listr_dup (params);
	if (self->params == NULL)
	{
		lisys_free (self);
		return NULL;
	}
	self->manager = manager;
	self->type = type;
	self->action = action;
	self->code = code;
	self->mods = mods;
	self->multiplier = multiplier;

	/* Calculate the priority of the binding. */
	for (mask = 0x1 ; mask != 0x0 ; mask <<= 1)
		self->priority += ((mods & mask) == mask);

	/* Add to manager. */
	if (manager->bindings != NULL)
		manager->bindings->prev = self;
	self->next = manager->bindings;
	self->prev = NULL;
	manager->bindings = self;

	return self;
}

/**
 * \brief Frees the binding.
 *
 * \param self Binding.
 */
void
libnd_binding_free (LIBndBinding* self)
{
	/* Remove from manager. */
	if (self->prev != NULL)
		self->prev->next = self->next;
	else
		self->manager->bindings = self->next;
	if (self->next != NULL)
		self->next->prev = self->prev;

	/* Free self. */
	lisys_free (self->params);
	lisys_free (self);
}

void*
libnd_binding_get_userdata (LIBndBinding* self)
{
	return self->userdata;
}

void
libnd_binding_set_userdata (LIBndBinding* self,
                            void*         value)
{
	self->userdata = value;
}

/** @} */
/** @} */
