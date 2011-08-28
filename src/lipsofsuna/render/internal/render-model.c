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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenModel Model
 * @{
 */

#include "render-model.h"

void liren_model_free (
	LIRenModel* self)
{
	lialg_u32dic_remove (self->render->models, self->id);
	if (self->v32 != NULL)
		liren_model32_free (self->v32);
	if (self->v21 != NULL)
		liren_model21_free (self->v21);
	lisys_free (self);
}

int liren_model_set_model (
	LIRenModel* self,
	LIMdlModel* model)
{
	if (self->v32 != NULL)
		return liren_model32_set_model (self->v32, model);
	else
		return liren_model21_set_model (self->v21, model);
}

/** @} */
/** @} */
/** @} */
