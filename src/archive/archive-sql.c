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
 * \addtogroup liarc Archive
 * @{
 * \addtogroup liarcSql Sql
 * @{
 */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <system/lips-system.h>
#include "archive-sql.h"

int
liarc_sql_delete (liarcSql*   self,
                  const char* table)
{
	int len;
	int pos;
	char* query;
	sqlite3_stmt* statement;

	/* Format query. */
	len = strlen (table);
	query = calloc (len + 14, sizeof (char));
	if (query == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}
	strcpy (query, "DELETE FROM ");
	pos = 12;
	strcpy (query + pos, table);
	pos += len;
	query[pos++] = ';';
	query[pos] = '\0';
	printf ("SQL DEBUG: %s\n", query);

	/* Execute statement. */
	if (sqlite3_prepare_v2 (self, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (self));
		return 0;
	}
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL: %s", sqlite3_errmsg (self));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	return 1;
}

int
liarc_sql_insert (liarcSql*   self,
                  const char* table,
                              ...)
{
	int i;
	int first;
	int intv;
	int num;
	int len;
	int pos;
	int type;
	float floatv;
	char* tmp;
	char* query;
	void* voidv;
	const char* strv;
	const char* field;
	va_list args;
	sqlite3_stmt* statement;

	/* Format query. */
	len = strlen (table);
	query = calloc (len + 15, sizeof (char));
	if (query == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}
	strcpy (query, "INSERT INTO ");
	pos = 12;
	strcpy (query + pos, table);
	pos += len;
	strcpy (query + pos, " (");
	pos += 2;
	va_start (args, table);
	first = 1;
	for (num = 0 ; 1 ; num++)
	{
		field = va_arg (args, char*);
		if (field == NULL)
			break;
		type = va_arg (args, int);
		switch (type)
		{
			case LIARC_SQL_BLOB:
				intv = va_arg (args, int);
				voidv = va_arg (args, void*);
				break;
			case LIARC_SQL_FLOAT:
				floatv = va_arg (args, double);
				break;
			case LIARC_SQL_INT:
				intv = va_arg (args, int);
				break;
			case LIARC_SQL_TEXT:
				strv = va_arg (args, char*);
				break;
		}
		len = strlen (field);
		tmp = realloc (query, pos + len + 2);
		if (tmp == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			free (query);
			return 0;
		}
		query = tmp;
		if (first)
			first = 0;
		else
			query[pos++] = ',';
		strcpy (query + pos, field);
		pos += len;
	}
	va_end (args);
	tmp = realloc (query, pos + num * 2 + 12);
	if (tmp == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		free (query);
		return 0;
	}
	query = tmp;
	strcpy (query + pos, ") VALUES (");
	pos += 10;
	for (i = 0 ; i < num ; i++)
	{
		query[pos++] = '?';
		if (i < num - 1)
			query[pos++] = ',';
		else
			query[pos++] = ')';
	}
	query[pos++] = ';';
	query[pos] = '\0';
	printf ("SQL DEBUG: %s\n", query);

	/* Prepare statement. */
	if (sqlite3_prepare_v2 (self, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (self));
		free (query);
		return 0;
	}
	free (query);

	/* Bind values. */
	va_start (args, table);
	for (i = 1 ; i <= num ; i++)
	{
		field = va_arg (args, char*);
		if (field == NULL)
			break;
		type = va_arg (args, int);
		switch (type)
		{
			case LIARC_SQL_BLOB:
				intv = va_arg (args, int);
				voidv = va_arg (args, void*);
				if (sqlite3_bind_blob (statement, i, voidv, intv, SQLITE_TRANSIENT) != SQLITE_OK)
				{
					lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (self));
					sqlite3_finalize (statement);
					return 0;
				}
				break;
			case LIARC_SQL_FLOAT:
				floatv = va_arg (args, double);
				if (sqlite3_bind_double (statement, i, floatv) != SQLITE_OK)
				{
					lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (self));
					sqlite3_finalize (statement);
					return 0;
				}
				break;
			case LIARC_SQL_INT:
				intv = va_arg (args, int);
				if (sqlite3_bind_int (statement, i, intv) != SQLITE_OK)
				{
					lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (self));
					sqlite3_finalize (statement);
					return 0;
				}
				break;
			case LIARC_SQL_TEXT:
				strv = va_arg (args, char*);
				if (sqlite3_bind_text (statement, i, strv, -1, SQLITE_TRANSIENT) != SQLITE_OK)
				{
					lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (self));
					sqlite3_finalize (statement);
					return 0;
				}
				break;
		}
	}
	va_end (args);

	/* Write values. */
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (self));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	return 1;
}

int
liarc_sql_replace (liarcSql*   self,
                   const char* table,
                               ...)
{
	int i;
	int first;
	int intv;
	int num;
	int len;
	int pos;
	int type;
	float floatv;
	char* tmp;
	char* query;
	void* voidv;
	const char* strv;
	const char* field;
	va_list args;
	sqlite3_stmt* statement;

	/* Format query. */
	len = strlen (table);
	query = calloc (len + 26, sizeof (char));
	if (query == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}
	strcpy (query, "INSERT OR REPLACE INTO ");
	pos = 12;
	strcpy (query + pos, table);
	pos += len;
	strcpy (query + pos, " (");
	pos += 2;
	va_start (args, table);
	first = 1;
	for (num = 0 ; 1 ; num++)
	{
		field = va_arg (args, char*);
		if (field == NULL)
			break;
		type = va_arg (args, int);
		switch (type)
		{
			case LIARC_SQL_BLOB:
				intv = va_arg (args, int);
				voidv = va_arg (args, void*);
				break;
			case LIARC_SQL_FLOAT:
				floatv = va_arg (args, double);
				break;
			case LIARC_SQL_INT:
				intv = va_arg (args, int);
				break;
			case LIARC_SQL_TEXT:
				strv = va_arg (args, char*);
				break;
		}
		len = strlen (field);
		tmp = realloc (query, pos + len + 2);
		if (tmp == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			free (query);
			return 0;
		}
		query = tmp;
		if (first)
			first = 0;
		else
			query[pos++] = ',';
		strcpy (query + pos, field);
		pos += len;
	}
	va_end (args);
	tmp = realloc (query, pos + num * 2 + 12);
	if (tmp == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		free (query);
		return 0;
	}
	query = tmp;
	strcpy (query + pos, ") VALUES (");
	pos += 10;
	for (i = 0 ; i < num ; i++)
	{
		query[pos++] = '?';
		if (i < num - 1)
			query[pos++] = ',';
		else
			query[pos++] = ')';
	}
	query[pos++] = ';';
	query[pos] = '\0';
	printf ("SQL DEBUG: %s\n", query);

	/* Prepare statement. */
	if (sqlite3_prepare_v2 (self, query, -1, &statement, NULL) != SQLITE_OK)
	{
		lisys_error_set (EINVAL, "SQL prepare: %s", sqlite3_errmsg (self));
		free (query);
		return 0;
	}
	free (query);

	/* Bind values. */
	va_start (args, table);
	for (i = 1 ; i <= num ; i++)
	{
		field = va_arg (args, char*);
		if (field == NULL)
			break;
		type = va_arg (args, int);
		switch (type)
		{
			case LIARC_SQL_BLOB:
				intv = va_arg (args, int);
				voidv = va_arg (args, void*);
				if (sqlite3_bind_blob (statement, i, voidv, intv, SQLITE_TRANSIENT) != SQLITE_OK)
				{
					lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (self));
					sqlite3_finalize (statement);
					return 0;
				}
				break;
			case LIARC_SQL_FLOAT:
				floatv = va_arg (args, double);
				if (sqlite3_bind_double (statement, i, floatv) != SQLITE_OK)
				{
					lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (self));
					sqlite3_finalize (statement);
					return 0;
				}
				break;
			case LIARC_SQL_INT:
				intv = va_arg (args, int);
				if (sqlite3_bind_int (statement, i, intv) != SQLITE_OK)
				{
					lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (self));
					sqlite3_finalize (statement);
					return 0;
				}
				break;
			case LIARC_SQL_TEXT:
				strv = va_arg (args, char*);
				if (sqlite3_bind_text (statement, i, strv, -1, SQLITE_TRANSIENT) != SQLITE_OK)
				{
					lisys_error_set (EINVAL, "SQL bind: %s", sqlite3_errmsg (self));
					sqlite3_finalize (statement);
					return 0;
				}
				break;
		}
	}
	va_end (args);

	/* Write values. */
	if (sqlite3_step (statement) != SQLITE_DONE)
	{
		lisys_error_set (EINVAL, "SQL step: %s", sqlite3_errmsg (self));
		sqlite3_finalize (statement);
		return 0;
	}
	sqlite3_finalize (statement);

	return 1;
}

/** @} */
/** @} */
