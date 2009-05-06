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
 * \addtogroup lical Callback
 * @{
 * \addtogroup licalMarshal Marshal
 * @{
 */

#include "cal-marshal.h"

int lical_marshal_DATA (void* call, void* data, va_list args)
{
	int (*func)(void*) = call;
	return func(data);
}

int lical_marshal_DATA_FLT (void* call, void* data, va_list args)
{
	int (*func)(void*, float) = call;
	double arg0 = va_arg (args, double);
	return func(data, arg0);
}

int lical_marshal_DATA_INT (void* call, void* data, va_list args)
{
	int (*func)(void*, int) = call;
	int arg0 = va_arg (args, int);
	return func(data, arg0);
}

int lical_marshal_DATA_INT_PTR (void* call, void* data, va_list args)
{
	int (*func)(void*, int, void*) = call;
	int arg0 = va_arg (args, int);
	void* arg1 = va_arg (args, void*);
	return func(data, arg0, arg1);
}

int lical_marshal_DATA_PTR (void* call, void* data, va_list args)
{
	int (*func)(void*, void*) = call;
	void* arg0 = va_arg (args, void*);
	return func(data, arg0);
}

int lical_marshal_DATA_PTR_INT (void* call, void* data, va_list args)
{
	int (*func)(void*, void*, int) = call;
	void* arg0 = va_arg (args, void*);
	int arg1 = va_arg (args, int);
	return func(data, arg0, arg1);
}

int lical_marshal_DATA_PTR_PTR (void* call, void* data, va_list args)
{
	int (*func)(void*, void*, void*) = call;
	void* arg0 = va_arg (args, void*);
	void* arg1 = va_arg (args, void*);
	return func(data, arg0, arg1);
}

int lical_marshal_DATA_PTR_PTR_INT (void* call, void* data, va_list args)
{
	int (*func)(void*, void*, void*, int) = call;
	void* arg0 = va_arg (args, void*);
	void* arg1 = va_arg (args, void*);
	int arg2 = va_arg (args, int);
	return func(data, arg0, arg1, arg2);
}

int lical_marshal_DATA_PTR_PTR_PTR (void* call, void* data, va_list args)
{
	int (*func)(void*, void*, void*, void*) = call;
	void* arg0 = va_arg (args, void*);
	void* arg1 = va_arg (args, void*);
	void* arg2 = va_arg (args, void*);
	return func(data, arg0, arg1, arg2);
}

/** @} */
/** @} */
