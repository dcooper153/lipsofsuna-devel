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

#ifndef __SYSTEM_EXECUTE_H__
#define __SYSTEM_EXECUTE_H__

#include <stdio.h>
#include "system-compiler.h"

typedef void (*lisysExecFilter)(void*, FILE*);

LIAPICALL (int, lisys_execvl, (
	const char* prog,
	            ...));

LIAPICALL (int, lisys_execvl_detach, (
	const char* prog,
	            ...));

LIAPICALL (int, lisys_execvl_redir_call, (
	lisysExecFilter call,
	void*           data,
	const char*     prog,
	                ...));

LIAPICALL (int, lisys_execvl_redir_file, (
	FILE*       file,
	const char* prog,
	            ...));

LIAPICALL (int, lisys_execvl_redir_path, (
	const char* path,
	const char* prog,
	            ...));

LIAPICALL (int, lisys_execvp, (
	const char*        prog,
	const char* const* args));

LIAPICALL (int, lisys_execvp_detach, (
	const char*        prog,
	const char* const* args));

LIAPICALL (int, lisys_execvp_redir_call, (
	lisysExecFilter    call,
	void*              data,
	const char*        prog,
	const char* const* args));

LIAPICALL (int, lisys_execvp_redir_file, (
	FILE*              file,
	const char*        prog,
	const char* const* args));

LIAPICALL (int, lisys_execvp_redir_path, (
	const char*        path,
	const char*        prog,
	const char* const* args));

#endif
