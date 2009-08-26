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
 * \addtogroup limai Main
 * @{
 * \addtogroup limaiMain Main
 * @{
 */

#include <string/lips-string.h>
#include <system/lips-system.h>

enum
{
	PRIVATE_MODE_CLIENT,
	PRIVATE_MODE_DEDICATED,
	PRIVATE_MODE_GENERATOR,
	PRIVATE_MODE_IMPORT,
	PRIVATE_MODE_VIEWER
};

static int
private_exec_program (const char* name,
                      const char* module,
                      const char* extra);

static int
print_help (const char* exe);

static int
client_main (const char* name);

static int
generator_main (const char* name);

static int
import_main (const char* name);

static int
server_main (const char* name);

static int
viewer_main (const char* name,
             const char* model);

/*****************************************************************************/

int main (int argc, char** argv)
{
	int i;
	int mode = PRIVATE_MODE_CLIENT;
	const char* model = NULL;
	const char* name = NULL;

	/* Parse arguments. */
	for (i = 1 ; i < argc ; i++)
	{
		if (!strcmp (argv[i], "-h") ||
		    !strcmp (argv[i], "--help"))
			return print_help (argv[0]);
		else if (!strcmp (argv[i], "-d") ||
		         !strcmp (argv[i], "--dedicated"))
			mode = PRIVATE_MODE_DEDICATED;
		else if (!strcmp (argv[i], "-g") ||
		         !strcmp (argv[i], "--generator"))
			mode = PRIVATE_MODE_GENERATOR;
		else if (!strcmp (argv[i], "-i") ||
		         !strcmp (argv[i], "--import"))
			mode = PRIVATE_MODE_IMPORT;
		else if (!strcmp (argv[i], "-m") ||
		         !strcmp (argv[i], "--model"))
		{
			if (i == argc - 1)
				return print_help (argv[0]);
			model = argv[++i];
			mode = PRIVATE_MODE_VIEWER;
		}
		else
			break;
	}

	/* Choose module. */
	if (i < argc - 1)
		return print_help (argv[0]);
	if (i == argc - 1)
		name = argv[i];
	else
		name = "data";

	/* Execute module. */
	switch (mode)
	{
		case PRIVATE_MODE_DEDICATED:
			return server_main (name);
		case PRIVATE_MODE_GENERATOR:
			return generator_main (name);
		case PRIVATE_MODE_IMPORT:
			return import_main (name);
		case PRIVATE_MODE_VIEWER:
			return viewer_main (name, model);
		default:
			return client_main (name);
	}
}

/*****************************************************************************/

static int
private_exec_program (const char* name,
                      const char* module,
                      const char* extra)
{
	int ret;
	char* tmp;
	char* path;

	/* Format path. */
#ifdef LI_RELATIVE_PATHS
	tmp = lisys_relative_exedir ();
	if (tmp == NULL)
		return 0;
	path = lisys_path_format (tmp,
		LISYS_PATH_SEPARATOR, "bin",
		LISYS_PATH_SEPARATOR, name, LISYS_EXTENSION_EXE, NULL);
	if (path == NULL)
	{
		lisys_free (tmp);
		return 0;
	}
#else
	tmp = LIPROGDIR;
	path = lisys_path_format (tmp, LISYS_PATH_SEPARATOR, name, LISYS_EXTENSION_EXE, NULL);
	if (path == NULL)
		return 0;
#endif

	/* Set library lookup path. */
#ifdef _WIN32
	char* env;
	char* val;
	env = getenv ("PATH");
	if (env != NULL)
		val = lisys_path_format ("PATH=", env, ";", tmp, LISYS_PATH_SEPARATOR, "lib", NULL);
	else
		val = lisys_path_format ("PATH=", tmp, LISYS_PATH_SEPARATOR, "lib", NULL);
	if (val != NULL)
	{
		if (putenv (val))
			lisys_free (val);
	}
#endif

	/* Execute the program. */
	ret = lisys_execvl (path, path, module, extra, NULL);
	lisys_free (path);
#ifdef LI_RELATIVE_PATHS
	lisys_free (tmp);
#endif
	if (!ret)
		return 0;

	return 1;
}

static int
print_help (const char* exe)
{
	printf ("Usage: %s [OPTION] [MODULE]\n\n", exe);
	printf ("  -d, --dedicated     Run as a dedicated server.\n");
	printf ("  -g, --generator     Run as a map generator.\n");
	printf ("  -h, --help          Display this help and exit.\n");
	printf ("  -i, --import        Import data files.\n\n");
	printf ("  -m, --model <name>  Run as a simple model viewer.\n\n");
	return 0;
}

static int
client_main (const char* name)
{
	if (!private_exec_program ("lipsofsuna-client", name, NULL))
	{
		lisys_error_append ("cannot execute client program");
		lisys_error_report ();
		return 1;
	}

	return 1;
}

static int
generator_main (const char* name)
{
	if (!private_exec_program ("lipsofsuna-generator", name, NULL))
	{
		lisys_error_append ("cannot execute generator program");
		lisys_error_report ();
		return 1;
	}

	return 1;
}

static int
import_main (const char* name)
{
	if (!private_exec_program ("lipsofsuna-import", name, NULL))
	{
		lisys_error_append ("cannot execute import program");
		lisys_error_report ();
		return 1;
	}

	return 1;
}

static int
server_main (const char* name)
{
	if (!private_exec_program ("lipsofsuna-server", name, NULL))
	{
		lisys_error_append ("cannot execute server program");
		lisys_error_report ();
		return 1;
	}

	return 0;
}

static int
viewer_main (const char* name,
             const char* model)
{
	if (!private_exec_program ("lipsofsuna-viewer", name, model))
	{
		lisys_error_append ("cannot execute viewer program");
		lisys_error_report ();
		return 1;
	}

	return 1;
}

/** @} */
/** @} */
