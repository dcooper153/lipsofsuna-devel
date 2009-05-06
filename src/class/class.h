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
 * \addtogroup licls Class
 * @{
 * \addtogroup liclsClass Class
 * @{
 */

#ifndef __CLASS_H__
#define __CLASS_H__

#include <stddef.h>
#include <assert.h>
#include "class-types.h"

/**
 * \brief Gets the base class of the class.
 *
 * \param self A class.
 * \return The base class or NULL.
 */
static inline const void* li_class_get_base (const void* self)
{
	const void* (*func)();
	const liclsClass* clss = self;

	if (clss->basetype == LI_CLASS_BASE_STATIC)
		return clss->base;
	assert (clss->basetype == LI_CLASS_BASE_DYNAMIC);
	func = clss->base;
	return func ();
}

/**
 * \brief Gets a non-NULL class pointer with inheritance.
 *
 * \param self The class defining the pointer.
 * \param type An inherited class.
 * \param offset The offset of the pointer.
 * \return A pointer or NULL.
 */
static inline void* li_class_get_pointer (const void* self,
                                          const void* type,
                                          size_t      offset)
{
	void* ptr;
	const liclsClass* clss;

	for (clss = type ; clss != NULL ; clss = li_class_get_base (clss))
	{
		ptr = *((void**)((void*) clss + offset));
		if (ptr != NULL)
			return ptr;
		if (clss == self)
			break;
	}

	return NULL;
}

#endif

/** @} */
/** @} */
