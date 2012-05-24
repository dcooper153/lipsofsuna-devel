/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#include "system-time.h"

/**
 * \brief Gets the current time.
 * \param t Return location for the time or NULL.
 * \return Time.
 */
time_t lisys_time (
	time_t* t)
{
	return time (t);
}

/**
 * \brief Gets the component breakdown of the given time.
 * \param t Time.
 * \param tm Time breakdown.
 */
void lisys_time_get_components (
	time_t         t,
	LISysTimeinfo* tm)
{
#if _POSIX_C_SOURCE >= 1 || _XOPEN_SOURCE || _BSD_SOURCE || _SVID_SOURCE || _POSIX_SOURCE
	localtime_r (&t, tm);
#else
	*tm = *localtime (&t);
#endif
}

/**
 * \brief Initializes the timeval from current time.
 * \return Timeval.
 */
LISysTimeval lisys_timeval_init ()
{
	struct timeval start;
	gettimeofday (&start, NULL);
	return start;
}

/**
 * \brief Calculates the difference in seconds between the timevals.
 * \param start Timeval.
 * \param end Timeval.
 * \return Seconds.
 */
float lisys_timeval_get_diff (
	LISysTimeval start,
	LISysTimeval end)
{
	return end.tv_sec - start.tv_sec +
		(end.tv_usec - start.tv_usec) * 0.000001f;
}

/** @} */
/** @} */
