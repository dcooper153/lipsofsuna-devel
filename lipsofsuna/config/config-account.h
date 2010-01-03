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
 * \addtogroup LICfgAccount Account
 * @{
 */

#ifndef __CONFIG_ACCOUNT_H__
#define __CONFIG_ACCOUNT_H__

typedef struct _LICfgAccount LICfgAccount;
struct _LICfgAccount
{
	char* password;
	unsigned int admin :1;
};

LICfgAccount*
licfg_account_new (const char* path);

void
licfg_account_free (LICfgAccount* self);

#endif

/** @} */
/** @} */

