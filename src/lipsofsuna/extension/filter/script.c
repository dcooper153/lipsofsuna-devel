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

/**
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtFilter Filter
 * @{
 */

#include "module.h"
#include "fir-filter.h"

static void FirFilter_new (LIScrArgs* args)
{
	int i;
	int length;
	float* coeffs;
	LIExtFirFilter* fir;
	LIScrData* data;

	/* Get the number of coefficients. */
	if (!liscr_args_geti_int (args, 0, &length))
		return;
	if (length <= 0)
		return;

	/* Allocate the coefficients. */
	coeffs = lisys_calloc (length, sizeof (float));
	if (!coeffs)
		return;

	/* Read the coefficients. */
	if (liscr_args_geti_table (args, 1))
	{
		for (i = 0 ; i < length ; i++)
		{
			lua_pushnumber (args->lua, i + 1);
			lua_gettable (args->lua, -2);
			if (lua_type (args->lua, -1) == LUA_TNUMBER)
				coeffs[i] = lua_tonumber (args->lua, -1);
			lua_pop (args->lua, 1);
		}
		lua_pop (args->lua, 1);
	}

	/* Create the filter. */
	fir = liext_fir_filter_new (length, coeffs);
	lisys_free (coeffs);
	if (fir == NULL)
		return;

	/* Allocate userdata. */
	data = liscr_data_new (args->script, args->lua, fir, LIEXT_SCRIPT_FIR_FILTER, liext_fir_filter_free);
	if (data == NULL)
	{
		liext_fir_filter_free (fir);
		return;
	}
	liscr_args_seti_stack (args);
}

static void FirFilter_add (LIScrArgs* args)
{
	float value;

	if (!liscr_args_geti_float (args, 0, &value))
		return;

	liext_fir_filter_add (args->self, value);
}

static void FirFilter_get (LIScrArgs* args)
{
	float output;

	output = liext_fir_filter_get (args->self);
	liscr_args_seti_float (args, output);
}

/*****************************************************************************/

void liext_script_fir_filter (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_FIR_FILTER, "fir_filter_new", FirFilter_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_FIR_FILTER, "fir_filter_add", FirFilter_add);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_FIR_FILTER, "fir_filter_get", FirFilter_get);
}

/** @} */
/** @} */
