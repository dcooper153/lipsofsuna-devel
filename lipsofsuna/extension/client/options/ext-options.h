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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliOptions Options 
 * @{
 */

#ifndef __EXT_OPTIONS_H__
#define __EXT_OPTIONS_H__

#include "ext-module.h"

#define LIEXT_WIDGET_OPTIONS(o) ((LIExtOptions*)(o))

typedef struct _LIExtOptions LIExtOptions;
struct _LIExtOptions
{
	LIWdgGroup base;
	LICliClient* client;
	LIWdgWidget* group;
	LIWdgWidget* check_fsaa;
	LIWdgWidget* check_global_shadows;
	LIWdgWidget* check_local_shadows;
	LIWdgWidget* check_shaders;
};

const LIWdgClass*
liext_widget_options ();

LIWdgWidget*
liext_options_new (LICliClient* client);

#endif

/** @} */
/** @} */
/** @} */
