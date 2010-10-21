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
 * \addtogroup liscrEvent Event
 * @{
 */

#ifndef __SCRIPT_EVENT_H__
#define __SCRIPT_EVENT_H__

#include <stdarg.h>
#include <lipsofsuna/script.h>
#include <lipsofsuna/system.h>

LIAPICALL (LIScrData*, liscr_event_new, (
	LIScrScript* script));

LIAPICALL (LIScrData*, liscr_event_newv, (
	LIScrScript* script,
	va_list      args));

LIAPICALL (LIScrData*, liscr_event_newva, (
	LIScrScript* script,
	             ...));

LIAPICALL (void, liscr_event_set, (
	LIScrData* self,
	           ...));

LIAPICALL (void, liscr_event_setv, (
	LIScrData* self,
	va_list    args));

LIAPICALL (const char*, liscr_event_get_type, (
	const LIScrData* self));

LIAPICALL (void, liscr_event_set_type, (
	LIScrData*  self,
	const char* type));

#endif

/** @} */
/** @} */
