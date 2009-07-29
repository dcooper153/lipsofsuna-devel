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
 * \addtogroup ligen Generator
 * @{
 * \addtogroup ligenMain Main
 * @{
 */

#include "generator.h"
#undef main

int
main (int argc, char** argv)
{
	ligenGenerator* self;

	if (argc != 3)
		return 1;
	srand (time (NULL));
	self = ligen_generator_new (argv[1], argv[2]);
	if (self == NULL)
	{
		lisys_error_report ();
		return 1;
	}
	if (!ligen_generator_main (self))
		lisys_error_report ();
	ligen_generator_free (self);

	return 0;
}

/** @} */
/** @} */
