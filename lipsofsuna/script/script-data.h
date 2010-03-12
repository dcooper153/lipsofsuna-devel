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
 * \addtogroup liscr Script
 * @{
 * \addtogroup LIScrData Data
 * @{
 */

#ifndef __SCRIPT_DATA_H__
#define __SCRIPT_DATA_H__

#include <lipsofsuna/system.h>
#include "script-types.h"

LIAPICALL (LIScrData*, liscr_data_new, (
	LIScrScript* script,
	void*        data,
	const char*  meta,
	liscrGCFunc  free));

LIAPICALL (LIScrData*, liscr_data_new_alloc, (
	LIScrScript* script,
	size_t       size,
	const char*  meta));

LIAPICALL (void, liscr_data_free, (
	LIScrData* object));

LIAPICALL (void, liscr_data_ref, (
	LIScrData* object,
	LIScrData* referencer));

LIAPICALL (void, liscr_data_unref, (
	LIScrData* object,
	LIScrData* referencer));

LIAPICALL (LIScrClass*, liscr_data_get_class, (
	LIScrData* self));

LIAPICALL (int, liscr_data_set_class, (
	LIScrData*  self,
	LIScrClass* clss));

LIAPICALL (LIScrScript*, liscr_data_get_script, (
	LIScrData* self));

LIAPICALL (int, liscr_data_get_valid, (
	LIScrData* self));

#endif

/** @} */
/** @} */
