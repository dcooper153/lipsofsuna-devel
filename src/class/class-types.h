/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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

#ifndef __CLASS_TYPES_H__
#define __CLASS_TYPES_H__

enum
{
	LI_CLASS_BASE_STATIC,
	LI_CLASS_BASE_DYNAMIC,
};

typedef struct _liclsClass liclsClass;
struct _liclsClass
{
	int basetype;
	const void* base;
	const void* name;
	int size;
	int (*init)(void*, void*);
	void (*free)(void*);
};

typedef union _liclsInstance liclsInstance;
union _liclsInstance
{
	const liclsClass* type;
	const liclsClass* type_;
};

#endif
