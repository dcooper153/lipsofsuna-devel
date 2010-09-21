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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtGenerator Generator
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "Extension.Generator"
 * --- Map region created by the generator.
 * -- @name Region
 * -- @class table
 */

/* @luadoc
 * --- Position of the region.
 * -- @name Region.point
 * -- @class table
 */
static void Region_getter_point (LIScrArgs* args)
{
	LIGenStroke* stroke;
	LIMatVector vector;

	stroke = args->self;
	vector = limat_vector_init (stroke->pos[0], stroke->pos[1], stroke->pos[2]);
	liscr_args_seti_vector (args, &vector);
}

/* @luadoc
 * --- Size of the region.
 * -- @name Region.size
 * -- @class table
 */
static void Region_getter_size (LIScrArgs* args)
{
	LIGenStroke* stroke;
	LIMatVector vector;

	stroke = args->self;
	vector = limat_vector_init (stroke->size[0], stroke->size[1], stroke->size[2]);
	liscr_args_seti_vector (args, &vector);
}

/* @luadoc
 * --- Type of the region.
 * -- @name Region.type
 * -- @class table
 */
static void Region_getter_type (LIScrArgs* args)
{
	LIGenStroke* stroke;

	stroke = args->self;
	liscr_args_seti_int (args, stroke->brush);
}

/*****************************************************************************/

void liext_script_generator_region (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_REGION, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_mvar (self, "point", Region_getter_point, NULL);
	liscr_class_insert_mvar (self, "size", Region_getter_size, NULL);
	liscr_class_insert_mvar (self, "type", Region_getter_type, NULL);
}

/** @} */
/** @} */
