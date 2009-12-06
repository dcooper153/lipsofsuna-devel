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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliScript Script
 * @{
 */

#ifndef __CLIENT_SCRIPT_H__
#define __CLIENT_SCRIPT_H__

#include <script/lips-script.h>
#include <widget/lips-widget.h>

#define LICLI_SCRIPT_ACTION "Lips.Action"
#define LICLI_SCRIPT_BINDING "Lips.Binding"
#define LICLI_SCRIPT_EXTENSION "Lips.Extension"
#define LICLI_SCRIPT_GROUP "Lips.Group"
#define LICLI_SCRIPT_LIGHT "Lips.Light"
#define LICLI_SCRIPT_MODULE "Lips.Module"
#define LICLI_SCRIPT_PLAYER "Lips.Player"
#define LICLI_SCRIPT_SCENE "Lips.Scene"
#define LICLI_SCRIPT_WIDGET "Lips.Widget"
#define LICLI_SCRIPT_WINDOW "Lips.Window"

void
licliActionScript (liscrClass* self,
                   void*       data);

void
licliBindingScript (liscrClass* self,
                    void*       data);

void
licliExtensionScript (liscrClass* self,
                      void*       data);

void
licliGroupScript (liscrClass* self,
                  void*       data);

void
licliLightScript (liscrClass* self,
                  void*       data);

void
licliModuleScript (liscrClass* self,
                   void*       data);

void
licliObjectScript (liscrClass* self,
                   void*       data);

void
licliPlayerScript (liscrClass* self,
                   void*       data);

void
licliSceneScript (liscrClass* self,
                  void*       data);

void
licliWidgetScript (liscrClass* self,
                   void*       data);

void
licliWindowScript (liscrClass* self,
                   void*       data);

/*****************************************************************************/

void
licli_script_widget_free (liwdgWidget* self,
                          liscrData*   data);

void
licli_script_widget_detach (liscrData* self);

void
licli_script_widget_detach_children (liscrData* self);

#endif

/** @} */
/** @} */
