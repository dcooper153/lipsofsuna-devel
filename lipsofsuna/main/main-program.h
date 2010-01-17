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
 * \addtogroup limai Main
 * @{
 * \addtogroup LIMaiProgram Program
 * @{
 */

#ifndef __MAIN_PROGRAM_H__
#define __MAIN_PROGRAM_H__

#include <sys/time.h>
#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/callback.h>
#include <lipsofsuna/engine.h>
#include <lipsofsuna/paths.h>
#include <lipsofsuna/script.h>
#include "main-extension.h"

typedef struct _LIMaiProgram LIMaiProgram;
struct _LIMaiProgram
{
	int sleep;
	int quit;
	struct timeval start;
	LIAlgSectors* sectors;
	LIAlgStrdic* components;
	LIAlgStrdic* extensions;
	LICalCallbacks* callbacks;
	LIEngEngine* engine;
	LIPthPaths* paths;
	LIScrScript* script;
};

LIMaiProgram*
limai_program_new (LIPthPaths* paths);

void
limai_program_free (LIMaiProgram* self);

int
limai_program_execute_script (LIMaiProgram* self,
                              const char*   file);

void*
limai_program_find_component (LIMaiProgram* self,
                              const char*   name);

LIMaiExtension*
limai_program_find_extension (LIMaiProgram* self,
                              const char*   name);

int
limai_program_insert_component (LIMaiProgram* self,
                                const char*   name,
                                void*         value);

int
limai_program_insert_extension (LIMaiProgram* self,
                                const char*   name);

int
limai_program_main (LIMaiProgram* self);

void
limai_program_remove_component (LIMaiProgram* self,
                                const char*   name);

void
limai_program_shutdown (LIMaiProgram* self);

int
limai_program_update (LIMaiProgram* self,
                      float         secs);

double
limai_program_get_time (const LIMaiProgram* self);

#endif

/** @} */
/** @} */
