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
 * \addtogroup licfg Config 
 * @{
 * \addtogroup licfgBans Bans
 * @{
 */

#ifndef __CONFIG_BANS_H__
#define __CONFIG_BANS_H__

#include <algorithm/lips-algorithm.h>

typedef struct _licfgBans licfgBans;
struct _licfgBans
{
	lialgStrdic* bans;
};

licfgBans* licfg_bans_new           ();
licfgBans* licfg_bans_new_from_file (const char* dir);
void       licfg_bans_free          (licfgBans*  self);
int        licfg_bans_insert_ban    (licfgBans*  self,
                                     const char* ip);
int        licfg_bans_remove_ban    (licfgBans*  self,
                                     const char* ip);
int        licfg_bans_get_banned    (licfgBans*  self,
                                     const char* ip);
int        licfg_bans_get_count     (licfgBans*  self);

#endif

/** @} */
/** @} */

