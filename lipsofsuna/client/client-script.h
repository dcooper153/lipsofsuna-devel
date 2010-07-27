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
#include <lipsofsuna/system.h>
#include <lipsofsuna/widget.h>

#define LICLI_SCRIPT_CLIENT "Client"
#define LICLI_SCRIPT_GROUP "Group"
#define LICLI_SCRIPT_LIGHT "Light"
#define LICLI_SCRIPT_SCENE "Scene"
#define LICLI_SCRIPT_WIDGET "Widget"

LIAPICALL (void, licli_script_client, (
	LIScrClass* self,
	void*       data));

LIAPICALL (void, licli_script_group, (
	LIScrClass* self,
	void*       data));

LIAPICALL (void, licli_script_light, (
	LIScrClass* self,
	void*       data));

LIAPICALL (void, licli_script_object, (
	LIScrClass* self,
	void*       data));

LIAPICALL (void, licli_script_scene, (
	LIScrClass* self,
	void*       data));

LIAPICALL (void, licli_script_widget, (
	LIScrClass* self,
	void*       data));

#endif

/** @} */
/** @} */
