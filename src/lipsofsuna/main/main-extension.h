/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

#ifndef __MAIN_EXTENSION_H__
#define __MAIN_EXTENSION_H__

#include <lipsofsuna/system.h>

#define LIMAI_EXTENSION_VERSION 0xFFFFFFFF

/**
 * \brief A structure for holding the information related to an extension.
 * This is used by the extension system to load and initialise extensions as they're needed.
 */
typedef struct _LIMaiExtensionInfo LIMaiExtensionInfo;
struct _LIMaiExtensionInfo
{
	unsigned int version; /**< The version of the extension system this extension was designed to work with.*/
	const char* name; /**< A string with the name of this extension.*/
	void* init; /**< A function pointer that's called to initialise this extension.*/
	void* free; /**< A function pointer that's called to release a previously initialised extension.*/
	void* memstat; /**< A function pointer to gather statistic's about the extension's memory usage.*/
};

typedef struct _LIMaiExtension LIMaiExtension;
struct _LIMaiExtension
{
	LIMaiExtensionInfo* info;
	LIMaiExtension* next;
	LISysModule* module;
	char name[64];
	void* object;
};

LIAPICALL (LIMaiExtensionInfo*, limai_extension_get_builtin, (
	const char* name));

#endif
