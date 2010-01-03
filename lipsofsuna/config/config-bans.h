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
 * \addtogroup licfg Config 
 * @{
 * \addtogroup LICfgBans Bans
 * @{
 */

#ifndef __CONFIG_BANS_H__
#define __CONFIG_BANS_H__

#include <lipsofsuna/algorithm.h>

typedef struct _LICfgBans LICfgBans;
struct _LICfgBans
{
	LIAlgStrdic* bans;
};

LICfgBans* licfg_bans_new           ();
LICfgBans* licfg_bans_new_from_file (const char* dir);
void       licfg_bans_free          (LICfgBans*  self);
int        licfg_bans_insert_ban    (LICfgBans*  self,
                                     const char* ip);
int        licfg_bans_remove_ban    (LICfgBans*  self,
                                     const char* ip);
int        licfg_bans_get_banned    (LICfgBans*  self,
                                     const char* ip);
int        licfg_bans_get_count     (LICfgBans*  self);

#endif

/** @} */
/** @} */

