/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

#ifndef __EXT_FILTER_FIR_FILTER_H__
#define __EXT_FILTER_FIR_FILTER_H__

#include "lipsofsuna/extension.h"

typedef struct _LIExtFirFilter LIExtFirFilter;
struct _LIExtFirFilter
{
	int dirty;
	int length;
	int offset;
	float output;
	float* coeffs;
	float* delay_line;
};

LIAPICALL (LIExtFirFilter*, liext_fir_filter_new, (
	int          length,
	const float* coeffs));

LIAPICALL (void, liext_fir_filter_free, (
	LIExtFirFilter* self));

LIAPICALL (void, liext_fir_filter_add, (
	LIExtFirFilter* self,
	float           sample));

LIAPICALL (float, liext_fir_filter_get, (
	LIExtFirFilter* self));

#endif
