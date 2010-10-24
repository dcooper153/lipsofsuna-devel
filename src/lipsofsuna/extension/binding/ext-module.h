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

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <lipsofsuna/binding.h>
#include <lipsofsuna/client.h>
#include <lipsofsuna/extension.h>

#define LIEXT_SCRIPT_ACTION "Action"
#define LIEXT_SCRIPT_BINDING "Binding"

typedef struct _LIExtModule LIExtModule;
struct _LIExtModule
{
	LIBndManager* bindings;
	LICalHandle calls[2];
	LICliClient* client;
	LIMaiProgram* program;
};

LIExtModule* liext_bindings_new (
	LIMaiProgram* program);

void liext_bindings_free (
	LIExtModule* self);

/*****************************************************************************/

void liext_script_action (
	LIScrClass* self,
	void*       data);

void liext_script_binding (
	LIScrClass* self,
	void*       data);

#endif
