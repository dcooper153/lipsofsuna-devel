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
 * \addtogroup liarc Archive
 * @{
 * \addtogroup LIArcSql Sql
 * @{
 */

#ifndef __ARCHIVE_SQL_H__
#define __ARCHIVE_SQL_H__

#include <sqlite3.h>

enum
{
	LIARC_SQL_BLOB,
	LIARC_SQL_FLOAT,
	LIARC_SQL_INT,
	LIARC_SQL_TEXT
};

typedef sqlite3 LIArcSql;

int
liarc_sql_delete (LIArcSql*   self,
                  const char* table);

int
liarc_sql_drop (LIArcSql*   self,
                const char* table);

int
liarc_sql_insert (LIArcSql*   self,
                  const char* table,
                              ...);

int
liarc_sql_query (LIArcSql*   self,
                 const char* query);

int
liarc_sql_replace (LIArcSql*   self,
                   const char* table,
                               ...);

#endif

/** @} */
/** @} */
