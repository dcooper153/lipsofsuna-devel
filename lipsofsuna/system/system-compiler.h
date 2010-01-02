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
 * \addtogroup lisys System
 * @{
 * \addtogroup lisysCompiler Compiler
 * @{
 */

#ifndef __SYSTEM_COMPILER_H__
#define __SYSTEM_COMPILER_H__

#if defined WIN32 && defined DLL_EXPORT
#define LIAPICALL __declspec(dllexport)
#else
#define LIAPICALL
#endif

#if __GNUC__ >= 4
#define __LI_ATTRIBUTE_CONST __attribute__((const))
#define __LI_ATTRIBUTE_FORMAT(i, j) __attribute__((format (printf, i, j)))
#define __LI_ATTRIBUTE_NORETURN __attribute__((noreturn))
#define __LI_ATTRIBUTE_SENTINEL __attribute__((sentinel))
#else
#define __LI_ATTRIBUTE_CONST
#define __LI_ATTRIBUTE_FORMAT(i, j)
#define __LI_ATTRIBUTE_NORETURN
#define __LI_ATTRIBUTE_SENTINEL
#endif

#endif

/** @} */
/** @} */

