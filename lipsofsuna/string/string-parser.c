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

#include <string/lips-string.h>
#include <system/lips-system.h>

liParser*
li_parser_new (void* data,
               int   length)
{
	liParser* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liParser));
	if (self == NULL)
		return NULL;

	/* Create reader. */
	self->reader = li_reader_new (data, length);
	if (self->reader == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

liParser*
li_parser_new_from_file (const char* path)
{
	liParser* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liParser));
	if (self == NULL)
		return NULL;

	/* Create reader. */
	self->reader = li_reader_new_from_file (path);
	if (self->reader == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

void
li_parser_free (liParser* self)
{
	li_reader_free (self->reader);
	lisys_free (self->tmp);
	lisys_free (self);
}

int
li_parser_next (liParser* self)
{
	int len;
	char* sep;
	char* word1_1;
	char* word1_2;
	char* word2_1;
	char* word2_2;

	if (self->error)
		return 0;
	lisys_free (self->tmp);
	self->tmp = NULL;
	self->name = NULL;
	self->value = NULL;

	while (1)
	{
		/* Check for end. */
		li_reader_skip_chars (self->reader, " \t\n");
		if (li_reader_check_end (self->reader))
		{
			if (self->depth != 0)
			{
				self->type = LI_PARSER_ERROR;
				self->error = EINVAL;
				return 1;
			}
			return 0;
		}

		/* Read line. */
		if (!li_reader_get_text (self->reader, "\n", &self->tmp))
		{
			self->error = lisys_error_get (NULL);
			self->type = LI_PARSER_ERROR;
			return 1;
		}
		len = strlen (self->tmp);

		/* Find the first word. */
		word1_1 = self->tmp;
		word1_2 = self->tmp + len;
		sep = strchr (word1_1, ' ');
		if (sep != NULL && sep < word1_2)
			word1_2 = sep;
		sep = strchr (word1_1, '\t');
		if (sep != NULL && sep < word1_2)
			word1_2 = sep;

		/* Find the remaining words. */
		word2_1 = word1_2;
		word2_2 = self->tmp + len;
		while (*word2_1 != '\0')
		{
			if (*word2_1 != ' ' && *word2_1 != '\t')
				break;
			word2_1++;
		}
		while (word2_1 < word2_2 - 1)
		{
			if (word2_2[-1] != ' ' && word2_2[-1] != '\t')
				break;
			word2_2--;
		}

		/* Terminate the substrings. */
		*word1_2 = '\0';
		*word2_2 = '\0';

		/* Check if skipped. */
		if (*word1_1 == '#')
			continue;
		if (*word1_1 == '\0' && *word2_1 == '\0')
			continue;

		/* Check if begin section. */
		if (!strcmp (word1_1, "begin"))
		{
			if (!strcmp (word2_1, ""))
			{
				self->type = LI_PARSER_ERROR;
				self->error = EINVAL;
				return 1;
			}
			self->type = LI_PARSER_BEGIN;
			self->name = word2_1;
			self->depth++;
			return 1;
		}

		/* Check if end section. */
		if (!strcmp (word1_1, "end"))
		{
			if (self->depth == 0 || strcmp (word2_1, ""))
			{
				self->type = LI_PARSER_ERROR;
				self->error = EINVAL;
				return 1;
			}
			self->type = LI_PARSER_END;
			self->depth--;
			return 1;
		}

		/* Check if value. */
		if (strcmp (word1_1, "") && strcmp (word2_1, ""))
		{
			self->type = LI_PARSER_VALUE;
			self->name = word1_1;
			self->value = word2_1;
			return 1;
		}

		/* Else bad syntax. */
		self->type = LI_PARSER_ERROR;
		self->error = EINVAL;
		return 1;
	}
}

/** @} */
/** @} */
