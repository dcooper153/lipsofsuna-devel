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
 * \addtogroup lisrvEvent Event
 * @{
 */

#ifndef __SERVER_EVENT_H__
#define __SERVER_EVENT_H__

enum
{
	LISRV_EVENT_TYPE_ACTION,
	LISRV_EVENT_TYPE_ANIMATION,
	LISRV_EVENT_TYPE_CONTROL,
	LISRV_EVENT_TYPE_EFFECT,
	LISRV_EVENT_TYPE_HEAR,
	LISRV_EVENT_TYPE_LOGIN,
	LISRV_EVENT_TYPE_LOGOUT,
	LISRV_EVENT_TYPE_MESSAGE,
	LISRV_EVENT_TYPE_PACKET,
	LISRV_EVENT_TYPE_SIMULATE,
	LISRV_EVENT_TYPE_SPEECH,
	LISRV_EVENT_TYPE_VISIBILITY
};

#endif

/** @} */
/** @} */
