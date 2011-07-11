/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LISys System
 * @{
 * \addtogroup LISysString String
 * @{
 */

#include <string.h>
#include <iconv.h>
#include "system-memory.h"

char* lisys_string_convert_sys_to_utf8 (
	const char* str)
{
	char* tmp;
	char* inbuf;
	char* outbuf;
	size_t n;
	size_t ninbytes;
	size_t noutbytes;
	iconv_t cd;

	/* Initialize buffers. */
	ninbytes = strlen (str);
	noutbytes = 4 * (ninbytes + 10);
	tmp = lisys_calloc (noutbytes, sizeof (char));
	if (tmp == NULL)
		return NULL;
	inbuf = (char*) str;
	outbuf = tmp;

	/* Initialize iconv. */
	cd = iconv_open ("UTF-8", "char");
	if (cd == (iconv_t) -1)
		cd = iconv_open ("UTF-8", "");
	if (cd == (iconv_t) -1)
	{
		lisys_free (tmp);
		return NULL;
	}

	/* Perform the conversion. */
	n = iconv (cd, &inbuf, &ninbytes, &outbuf, &noutbytes);
	iconv_close (cd);
	if (n == (size_t) -1)
	{
		lisys_free (tmp);
		return NULL;
	}

	return tmp;
}

/** @} */
/** @} */
