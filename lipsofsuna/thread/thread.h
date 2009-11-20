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
 * \addtogroup lithr Thread
 * @{
 * \addtogroup lithrThread Thread
 * @{
 */

#ifndef __THREAD_H__
#define __THREAD_H__

typedef struct _lithrThread lithrThread;
typedef void (*lithrThreadFunc)(lithrThread*, void*);

lithrThread*
lithr_thread_new (lithrThreadFunc func,
                  void*           data);

void
lithr_thread_free (lithrThread* self);

void
lithr_thread_join (lithrThread* self);

int
lithr_thread_get_done (lithrThread* self);

#endif

/** @} */
/** @} */
