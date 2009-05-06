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
 * \addtogroup liclsInstance Instance
 * @{
 */

#ifndef __CLASS_INSTANCE_H__
#define __CLASS_INSTANCE_H__

#include <stddef.h>
#include <stdlib.h>
#include "class.h"
#include "class-types.h"

static inline void* li_instance_new  (const void* type,
                                      void*       data);
static inline int   li_instance_init (void*       self,
                                      const void* type,
                                      void*       data);
static inline void  li_instance_free (void*       self,
                                      const void* type);

/*****************************************************************************/

/**
 * \brief Creates a new instance of the class.
 *
 * This function allocates a new object and calls the class constructor
 * for it if applicable.
 *
 * \param type A class.
 * \param data User data passed to the constructor.
 * \return A new instance or NULL.
 */
static inline void* li_instance_new (const void* type,
                                     void*       data)
{
	liclsInstance* inst;
	const liclsClass* clss = type;

	inst = calloc (1, clss->size);
	if (inst == NULL)
		return NULL;
	inst->type = type;
	if (!li_instance_init (inst, type, data))
	{
		free (inst);
		return NULL;
	}
	return inst;
}

static inline int li_instance_init (void*       self,
                                    const void* type,
                                    void*       data)
{
	const liclsClass* base;
	const liclsClass* clss = type;

	base = li_class_get_base (clss);
	if (base != NULL)
		li_instance_init (self, base, data);
	if (clss->init == NULL)
		return 1;
	if (clss->init (self, data))
		return 1;
	if (base != NULL)
		li_instance_free (self, base);
	return 0;
}

static inline void li_instance_free (void*       self,
                                     const void* type)
{
	const liclsClass* base;
	const liclsClass* clss = type;

	base = li_class_get_base (clss);
	if (clss->free != NULL)
		clss->free (self);
	if (base != NULL)
		li_instance_free (self, base);
}

static inline int li_instance_typeis (void*       self,
                                      const void* type)
{
	liclsInstance* inst = self;
	const liclsClass* clss;

	for (clss = inst->type ; clss != NULL ; clss = li_class_get_base (clss))
	{
		if (clss == type)
			return 1;
	}
	return 0;
}

#endif

/** @} */
/** @} */
