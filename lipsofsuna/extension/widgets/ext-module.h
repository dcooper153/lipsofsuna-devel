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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtWidgets Widgets
 * @{
 */

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include <lipsofsuna/client.h>
#include <lipsofsuna/widget.h>
#include <lipsofsuna/extension.h>

#define LIEXT_SCRIPT_BUTTON "Button"
#define LIEXT_SCRIPT_ENTRY "Entry"
#define LIEXT_SCRIPT_GROUP "Group"
#define LIEXT_SCRIPT_IMAGE "Image"
#define LIEXT_SCRIPT_LABEL "Label"
#define LIEXT_SCRIPT_MENU "Menu"
#define LIEXT_SCRIPT_SCENE "Scene"
#define LIEXT_SCRIPT_SCROLL "Scroll"
#define LIEXT_SCRIPT_SPIN "Spin"
#define LIEXT_SCRIPT_TREE "Tree"
#define LIEXT_SCRIPT_VIEW "View"
#define LIEXT_SCRIPT_WIDGET "Widget"
#define LIEXT_SCRIPT_WIDGETS "Widgets"

typedef struct _LIExtModule LIExtModule;
struct _LIExtModule
{
	LICalHandle calls[5];
	LICliClient* client;
	LIMaiProgram* program;
	LIWdgManager* widgets;
};

LIExtModule* liext_widgets_new (
	LIMaiProgram* program);

void liext_widgets_free (
	LIExtModule* self);

void liext_widgets_callback_paint (
	LIScrData* data);

/*****************************************************************************/

void
liext_script_button (LIScrClass* self,
                   void*       data);

void
liext_script_entry (LIScrClass* self,
                  void*       data);

void liext_script_group (
	LIScrClass* self,
	void*       data);

void
liext_script_image (LIScrClass* self,
                  void*       data);

void
liext_script_label (LIScrClass* self,
                  void*       data);

void
liext_script_menu (LIScrClass* self,
                 void*       data);

void liext_script_scene (
	LIScrClass* self,
	void*       data);

void
liext_script_scroll (LIScrClass* self,
                   void*       data);

void
liext_script_spin (LIScrClass* self,
                 void*       data);

void
liext_script_tree (LIScrClass* self,
                 void*       data);

void
liext_script_view (LIScrClass* self,
                 void*       data);

void liext_script_widget (
	LIScrClass* self,
	void*       data);

void liext_script_widgets (
	LIScrClass* self,
	void*       data);

#endif

/** @} */
/** @} */
