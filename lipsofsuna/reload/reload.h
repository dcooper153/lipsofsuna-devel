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
 * \addtogroup lirel Reload
 * @{
 * \addtogroup LIRelReload Reload
 * @{
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
	LIThrAsyncCall* worker;
	void (*reload_image_call)(void*, const char*);
	void* reload_image_data;
	void (*reload_model_call)(void*, const char*);
	void* reload_model_data;
};

LIRelReload*
lirel_reload_new (LIPthPaths* paths);

void
lirel_reload_free (LIRelReload* self);

void
lirel_reload_cancel (LIRelReload* self);

int
lirel_reload_main (LIRelReload* self);

int
lirel_reload_run (LIRelReload* self);

int
lirel_reload_update (LIRelReload* self);

int
lirel_reload_get_done (const LIRelReload* self);

int
lirel_reload_get_enabled (const LIRelReload* self);

int
lirel_reload_set_enabled (LIRelReload* self,
                          int          value);

void
lirel_reload_set_image_callback (LIRelReload* self,
                                 void       (*call)(),
                                 void*        data);

void
lirel_reload_set_model_callback (LIRelReload* self,
                                 void       (*call)(),
                                 void*        data);

float
lirel_reload_get_progress (const LIRelReload* self);

/**
 * @}
 * \addtogroup lirelBlender Blender
 * @{
 */

int
lirel_reload_blender (LIRelReload* self,
                      const char*  src,
                      const char*  dst);

/**
 * @}
 * \addtogroup lirelImage Image
 * @{
 */

int
lirel_reload_image (LIRelReload* self,
                    const char*  src,
                    const char*  dst);

/**
 * @}
 * \addtogroup lirelGimp Gimp
 * @{
 */

int
lirel_reload_gimp (LIRelReload* self,
                   const char*  src,
                   const char*  dst);

#endif

/** @} */
/** @} */
