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

#ifndef __RELOAD_H__
#define __RELOAD_H__

#include <lipsofsuna/paths.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/thread.h>

typedef struct _LIRelReload LIRelReload;
struct _LIRelReload
{
	int queued;
	LIPthPaths* paths;
	LISysNotify* notify;
	void (*reload_image_call)(void*, const char*);
	void* reload_image_data;
	void (*reload_model_call)(void*, const char*);
	void* reload_model_data;
};

LIAPICALL (LIRelReload*, lirel_reload_new, (
	LIPthPaths* paths));

LIAPICALL (void, lirel_reload_free, (
	LIRelReload* self));

LIAPICALL (int, lirel_reload_update, (
	LIRelReload* self));

LIAPICALL (int, lirel_reload_get_enabled, (
	const LIRelReload* self));

LIAPICALL (int, lirel_reload_set_enabled, (
	LIRelReload* self,
	int          value));

LIAPICALL (void, lirel_reload_set_image_callback, (
	LIRelReload* self,
	void       (*call)(),
	void*        data));

LIAPICALL (void, lirel_reload_set_model_callback, (
	LIRelReload* self,
	void       (*call)(),
	void*        data));

#endif
