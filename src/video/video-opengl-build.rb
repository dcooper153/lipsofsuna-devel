#! /usr/bin/ruby

apidefs = []
extdefs = []
typedefs = []
apifuncs = []
extfuncs = []

line = ''
start = false
file = File.new("gl.h", "r")
file.each_line do |l|
	if l['extern "C" {']
		start = true
		next
	end
	if start

		# Merge incomplete lines.
		if line[line.length - 1] == ','[0]
			line = line + l
		else
			line = l
		end

		# Strip extra spaces.
		line.gsub!(/\/\*.*\*\//, '')
		line.tr!("\t\n", ' ')
		line.squeeze!(' ')
		line.strip!
		line.gsub!(' )', ')')
		line.gsub!(' (', '(')
		line.gsub!('( ', '(')
		line.gsub!(') ', ')')
		if line[line.length - 1] == ','[0]
			next
		end

		# Don't copy extensions.
		if line['OpenGL 1.2']
			break
		end

		# Extract interesting stuff.
		if line['#define GL_']
			if line[/ 1$/]
				extdefs.push(line)
			else
				apidefs.push(line)
			end
		elsif l['typedef'] and line[' GL'] and not line['APIENTRYP']
			typedefs.push(line)
		elsif line['GLAPI'] and line['GLAPIENTRY gl']
			line['('] = ')('
			line['GLAPIENTRY '] = 'GLAPIENTRY('
			apifuncs.push(line)
		end

	end
end
file.close

line = ''
level = 0
ignore = 0
start = false
file = File.new("glext.h", "r")
file.each_line do |l|
	if l['/* Current version at http://www.opengl.org/registry/ */']
		start = true
		next
	end
	if start

		# Merge incomplete lines.
		if line[line.length - 1] == ','[0]
			line = line + l
		else
			line = l
		end

		# Strip extra spaces.
		line.gsub!(/\/\*.*\*\//, '')
		line.tr!("\t\n", ' ')
		line.squeeze!(' ')
		line.strip!
		line.gsub!(' )', ')')
		line.gsub!(' (', '(')
		line.gsub!('( ', '(')
		line.gsub!(') ', ')')
		if line[line.length - 1] == ','[0]
			next
		end

		# Track ifdefs.
		if line['#ifdef'] or line['#ifndef']
			level = level + 1
			if ignore != 0 or (not line['GL_VERSION'] and not line['GL_EXT_'] and not line['GL_ARB_'])
				ignore = ignore + 1
			end
		end
		if line['#endif']
			level = level - 1
			if ignore > 0
				ignore = ignore - 1
			end
		end

		# Extract interesting stuff.
		if line['#define GL_']
			if ignore <= 0
				if line[/ 1$/]
					extdefs.push(line)
				else
					apidefs.push(line)
				end
			end
		elsif line['typedef'] and line[' GL'] and not line['APIENTRYP']
			typedefs.push(line)
		elsif line['GLAPI'] and line['APIENTRY gl'] and (line['ARB('] or line['EXT('])
			line['('] = ')('
			line['APIENTRY '] = 'GLEXTENTRY('
			extfuncs.push(line)
		end
	end
end
file.close

apidefs.sort!
extdefs.sort!
typedefs.sort!
apifuncs.sort!
extfuncs.sort!
apidefs.uniq!
extdefs.uniq!
typedefs.uniq!
apifuncs.uniq!
extfuncs.uniq!
apifuncs.delete_if do |l|
	a = (l =~ /\(/) + 1
	b = (l =~ /\)/)
	name = l[a, b - a]
	dupe = false
	extfuncs.each do |k|
		a = (k =~ /\(/) + 1
		b = (k =~ /\)/)
		tmp = k[a, b - a]
		if name == tmp then
			dupe = true
			break
		end
	end
	dupe
end

file = File.new("video-opengl.h", "w")
file.print(<<END
/*
 * Copyright (C) 1999-2007  Brian Paul   All Rights Reserved.
 * Copyright (C) 2007  The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 */

#ifndef __VIDEO_OPENGL_H__
#define __VIDEO_OPENGL_H__
#define __gl_h_
#define __glext_h_

int
livid_video_init (const char* path);

void
livid_video_free ();

int
livid_video_check_support (const char* extension);

#include <stddef.h>
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#include <inttypes.h>
#elif defined(__sun__)
#include <inttypes.h>
#if defined(__STDC__)
#if defined(__arch64__)
typedef long int int64_t;
typedef unsigned long int uint64_t;
#else
typedef long long int int64_t;
typedef unsigned long long int uint64_t;
#endif
#endif
#elif defined( __VMS )
#include <inttypes.h>
#elif defined(__SCO__) || defined(__USLC__)
#include <stdint.h>
#elif defined(__UNIXOS2__) || defined(__SOL64__)
typedef long int int32_t;
typedef long long int int64_t;
typedef unsigned long long int uint64_t;
#elif defined(_WIN32) && defined(__GNUC__)
#include <stdint.h>
#elif defined(_WIN32)
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
#include <inttypes.h>
#endif

#ifndef GLAPI
#define GLAPI extern
#endif
#define GLAPIENTRY(fun) fun
#define GLEXTENTRY(fun) (*fun)

END
)
extfuncs.each do |l|
	a = (l =~ /\(/) + 1
	b = (l =~ /\)/)
	name = l[a, b - a]
	file.print("#define " + name + ' livid_' + name + "\n")
end
file.print("\n")
apidefs.each do |l|
	file.print(l + "\n")
end
file.print("\n")
extdefs.each do |l|
	file.print(l + "\n")
end
file.print("\n")
typedefs.each do |l|
	file.print(l + "\n")
end
file.print("\n")
apifuncs.each do |l|
	file.print(l + "\n")
end
file.print("\n")
extfuncs.each do |l|
	file.print(l + "\n")
end
file.print(<<END

#endif

END
)

file.close

#############################################################################

file = File.new("video-opengl.c", "w")
file.print(<<END
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

#define GLAPI
#include <dlfcn.h>
#include <string.h>
#include <SDL/SDL.h>
#include "video-opengl.h"

static int initialized = 0;

/**
 * \\brief Initializes the video subsystem.
 *
 * \\param path Path to the OpenGL library or NULL.
 * \\return Nonzero on success.
 */
int
livid_video_init (const char* path)
{
	if (initialized++)
		return 1;
	SDL_Init (SDL_INIT_VIDEO);

END
)
extfuncs.each do |l|
	a = (l =~ /\(/) + 1
	b = (l =~ /\)/)
	name = l[a, b - a]
	#file.print("\t" + name + ' = SDL_GL_GetProcAddress ("' + name + "\");\n")
	file.print("\t" + name + ' = dlsym (RTLD_DEFAULT, "' + name + "\");\n")
end
file.print(<<END
	return 1;
}

/**
 * \\brief Deinitializes the video subsystem.
 */
void
livid_video_free ()
{
	initialized--;
}

/**
 * \\brief Checks if an extension is supported.
 *
 * \\param extension Extension string.
 * \\return Nonzero if supported.
 */
int
livid_video_check_support (const char* extension)
{
	const GLubyte* extensions = NULL;
	const GLubyte* start;
	GLubyte* where;
	GLubyte* terminator;

	where = (GLubyte*) strchr (extension, ' ');
	if (where || *extension == '\\0')
		return 0;
	extensions = glGetString (GL_EXTENSIONS);
	start = extensions;
	while (1)
	{
		where = (GLubyte*) strstr ((const char*) start, extension);
		if (!where)
			break;
		terminator = where + strlen (extension);
		if (where == start || *(where - 1) == ' ')
		{
			if (*terminator == ' ' || *terminator == '\\0')
				return 1;
		}
		start = terminator;
	}
	return 0;
}

END
)

file.close

