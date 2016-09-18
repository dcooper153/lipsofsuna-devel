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

#ifndef __SYSTEM_COMPILER_H__
#define __SYSTEM_COMPILER_H__

/**
 * \addtogroup LISys System
 * @{
 * \addtogroup LISysCompiler Compiler
 * @{
 */
#ifdef WIN32
 #ifdef __cplusplus
  #define LIAPIEXPORT(ret, name, args) extern "C" __declspec(dllexport) ret name args
  #define LIAPIIMPORT(ret, name, args) extern "C" __declspec(dllimport) ret name args
 #else
  #define LIAPIEXPORT(ret, name, args) __declspec(dllexport) ret name args
  #define LIAPIIMPORT(ret, name, args) __declspec(dllimport) ret name args
 #endif
#else
 #ifdef __cplusplus
  #define LIAPIEXPORT(ret, name, args) extern "C" ret name args
  #define LIAPIIMPORT(ret, name, args) extern "C" ret name args
 #else
  #define LIAPIEXPORT(ret, name, args) ret name args
  #define LIAPIIMPORT(ret, name, args) ret name args
 #endif
#endif

/**
 * \brief Declares a C function.
 * \param ret The return expression of the function.
 * \param name The name of the function.
 * \param args
 */
#ifdef DLL_EXPORT
 #define LIAPICALL(ret, name, args) LIAPIIMPORT(ret, name, args)
#else
 #define LIAPICALL(ret, name, args) LIAPIEXPORT(ret, name, args)
#endif

/**
 * \brief Declares a GCC style attribute.
 * For compilers not supporting this, it will be an empty macro.
 * \param attr The attribute to declare.
 */
#if __GNUC__ >= 4
#define LISYS_ATTR(attr) __attribute__(attr)
#else
#define LISYS_ATTR(attr)
#endif

/**
 * \brief Declare a function as having the const attribute.
 */
#define LISYS_ATTR_CONST LISYS_ATTR((const))
/**
 * \brief Declare a function as having printf style formatting.
 * \param i The index of the parameter with the format string (1 based).
 * \param j The index of the first vararg parameter (1 based).
 */
#define LISYS_ATTR_FORMAT(i, j) LISYS_ATTR((format (printf, i, j)))
/**
 * \brief Declare a function as never returning.
 */
#define LISYS_ATTR_NORETURN LISYS_ATTR((noreturn))
/**
 * \brief Declare a function as the sentinel attribute.
 */
#define LISYS_ATTR_SENTINEL LISYS_ATTR((sentinel))

/** @} */
/** @} */

#endif

