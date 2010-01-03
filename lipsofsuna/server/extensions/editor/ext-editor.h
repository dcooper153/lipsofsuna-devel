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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvEditor Editor      
 * @{
 */

#ifndef __EXT_EDITOR_H__
#define __EXT_EDITOR_H__

#include <lipsofsuna/server.h>

typedef struct _LIExtEditor LIExtEditor;
struct _LIExtEditor
{
	LISerServer* server;
	LICalHandle calls[1];
};

LIExtEditor*
liext_editor_new (LISerServer* server);

void
liext_editor_free (LIExtEditor* self);

#endif

/** @} */
/** @} */
/** @} */
