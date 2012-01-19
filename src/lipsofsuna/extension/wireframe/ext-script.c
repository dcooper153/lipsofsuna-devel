/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIExtWireframe Wireframe
 * @{
 */

#include "ext-module.h"

static void Wireframe_on (LIScrArgs* args)
{
	printf ("FIXME: Wireframe_on is disabled due to Ogre transition\n");
//	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
}

static void Wireframe_off (LIScrArgs* args)
{
	printf ("FIXME: Wireframe_off is disabled due to Ogre transition\n");
//	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

/*****************************************************************************/

void liext_script_wireframe (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_WIREFRAME, "wireframe_on", Wireframe_on);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_WIREFRAME, "wireframe_off", Wireframe_off);
}

/** @} */
/** @} */
