/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup lisys System
 * @{
 * \addtogroup lisysNotify Notify
 * @{
 */

#ifndef __SYSTEM_NOTIFY_H__
#define __SYSTEM_NOTIFY_H__

typedef int lisysNotifyFlags;
enum _lisysNotifyFlags
{
	LISYS_NOTIFY_CLOSER = 0x01,
	LISYS_NOTIFY_CLOSEW = 0x02,
	LISYS_NOTIFY_CLOSE = 0x03,
	LISYS_NOTIFY_CREATE = 0x04,
	LISYS_NOTIFY_DELETE = 0x08,
	LISYS_NOTIFY_MODIFY = 0x10,
	LISYS_NOTIFY_ALL = 0xFF
};

typedef struct _lisysNotify lisysNotify;
struct _lisysNotify
{
	int fd;
	struct
	{
		int pos;
		int length;
		int capacity;
		void* buffer;
	} buffer;
	struct
	{
		int dir;
		int write;
		const char* name;
		lisysNotifyFlags flags;
	} event;
};

lisysNotify*
lisys_notify_new ();

void
lisys_notify_free (lisysNotify* self);

int
lisys_notify_add (lisysNotify*     self,
                  const char*      path,
                  lisysNotifyFlags flags);

void
lisys_notify_remove (lisysNotify* self,
                     const char*  path);

int
lisys_notify_poll (lisysNotify* self);

int
lisys_notify_wait (lisysNotify* self);

#endif

/** @} */
/** @} */
