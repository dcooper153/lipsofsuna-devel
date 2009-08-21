/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup lirelReload Reload
 * @{
 */

#ifndef __RELOAD_H__
#define __RELOAD_H__

#include <paths/lips-paths.h>
#include <system/lips-system.h>
#include <thread/lips-thread.h>

typedef struct _lirelReload lirelReload;
struct _lirelReload
{
	int queued;
	lipthPaths* paths;
	lisysNotify* notify;
	lithrAsyncCall* worker;
	void (*reload_image_call)(void*, const char*);
	void* reload_image_data;
	void (*reload_model_call)(void*, const char*);
	void* reload_model_data;
};

lirelReload*
lirel_reload_new (lipthPaths* paths);

void
lirel_reload_free (lirelReload* self);

void
lirel_reload_cancel (lirelReload* self);

int
lirel_reload_main (lirelReload* self);

int
lirel_reload_run (lirelReload* self);

int
lirel_reload_update (lirelReload* self);

int
lirel_reload_get_done (const lirelReload* self);

int
lirel_reload_get_enabled (const lirelReload* self);

int
lirel_reload_set_enabled (lirelReload* self,
                          int          value);

void
lirel_reload_set_image_callback (lirelReload* self,
                                 void       (*call)(),
                                 void*        data);

void
lirel_reload_set_model_callback (lirelReload* self,
                                 void       (*call)(),
                                 void*        data);

float
lirel_reload_get_progress (const lirelReload* self);

/**
 * @}
 * \addtogroup lirelBlender Blender
 * @{
 */

int
lirel_reload_blender (lirelReload* self,
                      const char*  src,
                      const char*  dst);

/**
 * @}
 * \addtogroup lirelImage Image
 * @{
 */

int
lirel_reload_image (lirelReload* self,
                    const char*  src,
                    const char*  dst);

/**
 * @}
 * \addtogroup lirelGimp Gimp
 * @{
 */

int
lirel_reload_gimp (lirelReload* self,
                   const char*  src,
                   const char*  dst);

#endif

/** @} */
/** @} */
