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
 * \addtogroup listr String
 * @{
 * \addtogroup listrParser Parser
 * @{
 */

#ifndef __STRING_PARSER_H__
#define __STRING_PARSER_H__

#include "string-reader.h"

enum
{
	LI_PARSER_ERROR,
	LI_PARSER_BEGIN,
	LI_PARSER_END,
	LI_PARSER_VALUE,
};

typedef struct _liParser liParser;
struct _liParser
{
	int depth;
	int type;
	int error;
	const char* name;
	const char* value;
	liReader* reader;
	char* tmp;
};

liParser*
li_parser_new (void* data,
               int   length);

liParser*
li_parser_new_from_file (const char* path);

void
li_parser_free (liParser* self);

int
li_parser_next (liParser* self);

#endif

/** @} */
/** @} */
