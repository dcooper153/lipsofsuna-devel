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
 * \addtogroup lisrv Server
 * @{
 * \addtogroup lisrvObject Object
 * @{
 */

#ifndef __SERVER_OBJECT_H__
#define __SERVER_OBJECT_H__

#include <ai/lips-ai.h>
#include <archive/lips-archive.h>
#include <engine/lips-engine.h>
#include <script/lips-script.h>
#include <string/lips-string.h>
#include "server-types.h"

#define LISRV_OBJECT(o) ((lisrvObject*) lieng_object_get_userdata ((liengObject*)(o), LIENG_DATA_SERVER))

#define LISRC_OBJECT_POSITION_ERROR 0.4f
#define LISRC_OBJECT_VELOCITY_ERROR 1.0f
#define LISRC_OBJECT_DIRECTION_ERROR 0.01f

struct _lisrvObject
{
	char* name;
	uint8_t flags;
	lialgU32dic* animations;
	lisrvClient* client;
	lisrvServer* server;
};

liengObject*
lisrv_object_new (liengEngine*     engine,
                  liengModel*      model,
                  liphyShapeMode   shape_mode,
                  liphyControlMode control_mode,
                  uint32_t         id,
                  void*            ptr);

void
lisrv_object_free (liengObject* self);

int
lisrv_object_animate (liengObject* self,
                      const char*  name,
                      int          channel,
                      float        priority,
                      int          permanent);

void
lisrv_object_disconnect (liengObject* self);

void
lisrv_object_effect (liengObject* self,
                     const char*  value,
                     int          flags);

int
lisrv_object_moved (liengObject* self);

void
lisrv_object_say (liengObject* self,
                  const char*  value);

int
lisrv_object_sees (const liengObject* self,
                   const liengObject* target);

int
lisrv_object_serialize (liengObject*    self,
                        liarcSerialize* serializer);

liaiPath*
lisrv_object_solve_path (const liengObject* self,
                         const limatVector*    target);

void
lisrv_object_swap (liengObject* self,
                   liengObject* object);

int
lisrv_object_set_client (liengObject* self,
                         lisrvClient* value);

uint32_t
lisrv_object_get_model (const liengObject* self);

int
lisrv_object_set_model (liengObject* self,
                        liengModel*  value);

const char*
lisrv_object_get_name (const liengObject* self);

int
lisrv_object_set_name (liengObject* self,
                       const char*  value);

int
lisrv_object_set_realized (liengObject* self,
                           int          value);

int
lisrv_object_set_transform (liengObject*          self,
                            const limatTransform* transform);

int
lisrv_object_get_valid (const liengObject* self);

int
lisrv_object_set_velocity (liengObject*       self,
                           const limatVector* value);

#endif

/** @} */
/** @} */
