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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliScript Script
 * @{
 */

#ifndef __CLIENT_SCRIPT_H__
#define __CLIENT_SCRIPT_H__

#include <lipsofsuna/script.h>
#include <lipsofsuna/widget.h>

#define LICLI_SCRIPT_ACTION "Lips.Action"
#define LICLI_SCRIPT_BINDING "Lips.Binding"
#define LICLI_SCRIPT_CLIENT "Lips.Client"
#define LICLI_SCRIPT_EXTENSION "Lips.Extension"
#define LICLI_SCRIPT_GROUP "Lips.Group"
#define LICLI_SCRIPT_LIGHT "Lips.Light"
#define LICLI_SCRIPT_PLAYER "Lips.Player"
#define LICLI_SCRIPT_SCENE "Lips.Scene"
#define LICLI_SCRIPT_WIDGET "Lips.Widget"
#define LICLI_SCRIPT_WINDOW "Lips.Window"

void
licli_script_action (LIScrClass* self,
                   void*       data);

void
licli_script_binding (LIScrClass* self,
                    void*       data);

void
licli_script_client (LIScrClass* self,
                   void*       data);

void
licli_script_extension (LIScrClass* self,
                      void*       data);

void
licli_script_group (LIScrClass* self,
                  void*       data);

void
licli_script_light (LIScrClass* self,
                  void*       data);

void
licli_script_object (LIScrClass* self,
                   void*       data);

void
licli_script_player (LIScrClass* self,
                   void*       data);

void
licli_script_scene (LIScrClass* self,
                  void*       data);

void
licli_script_widget (LIScrClass* self,
                   void*       data);

void
licli_script_window (LIScrClass* self,
                   void*       data);

/*****************************************************************************/

void
licli_script_widget_free (LIWdgWidget* self,
                          LIScrData*   data);

void
licli_script_widget_detach (LIScrData* self);

void
licli_script_widget_detach_children (LIScrData* self);

#endif

/** @} */
/** @} */
