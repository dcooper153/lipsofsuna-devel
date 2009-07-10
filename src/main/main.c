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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <system/lips-system.h>

enum
{
	PRIVATE_MODE_CLIENT,
	PRIVATE_MODE_DEDICATED,
	PRIVATE_MODE_GENERATOR,
	PRIVATE_MODE_VIEWER
};

static lisysModule*
private_open_program (const char* name);

static int
print_help (const char* exe);

static int
client_main (const char* name);

static int
generator_main (const char* name);

static int
server_main (const char* name);

static int
viewer_main (const char* name,
             const char* model);

/*****************************************************************************/

int
main (int argc, char** argv)
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
	srand (time (NULL));
	switch (mode)
	{
		case PRIVATE_MODE_DEDICATED:
			return server_main (name);
		case PRIVATE_MODE_GENERATOR:
			return generator_main (name);
		case PRIVATE_MODE_VIEWER:
			return viewer_main (name, model);
		default:
			return client_main (name);
	}
}

/*****************************************************************************/

static lisysModule*
private_open_program (const char* name)
{
	char* tmp;
	char* path;
	lisysModule* module;

	/* Format path. */
#ifdef LI_RELATIVE_PATHS
	tmp = lisys_relative_exedir ();
	if (tmp == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	path = lisys_path_format (tmp,
		LISYS_PATH_SEPARATOR, "lib",
		LISYS_PATH_SEPARATOR, "programs",
		LISYS_PATH_SEPARATOR, "lib", name, ".", LISYS_EXTENSION_DLL, NULL);
	free (tmp);
	if (path == NULL)
		return NULL;
#else
	tmp = LIPROGDIR;
	path = lisys_path_format (tmp, LISYS_PATH_SEPARATOR, "lib", name, ".", LISYS_EXTENSION_DLL, NULL);
	if (path == NULL)
		return NULL;
#endif

	/* Open library. */
	module = lisys_module_new (path, LISYS_MODULE_FLAG_GLOBAL | LISYS_MODULE_FLAG_LIBDIRS);
	free (path);

	return module;
}

static int
print_help (const char* exe)
{
	printf ("Usage: %s [OPTION] [MODULE]\n\n", exe);
	printf ("  -d, --dedicated     Run as a dedicated server.\n");
	printf ("  -g, --generator     Run as a map generator.\n");
	printf ("  -h, --help          Display this help and exit.\n");
	printf ("  -m, --model <name>  Run as a simple model viewer.\n\n");
	return 0;
}

static int
client_main (const char* name)
{
	void* client;
	void (*licli_client_free)(void*);
	int (*licli_client_main)(void*);
	void* (*licli_client_new)(const char*);
	lisysModule* module;

	/* Open client library. */
	module = private_open_program ("lipsofsunaclient");
	if (module == NULL)
	{
		lisys_error_append ("cannot load client program");
		lisys_error_report ();
		return 1;
	}

	/* Find used functions. */
	licli_client_free = lisys_module_symbol (module, "licli_client_free");
	licli_client_main = lisys_module_symbol (module, "licli_client_main");
	licli_client_new = lisys_module_symbol (module, "licli_client_new");
	if (licli_client_free == NULL ||
	    licli_client_main == NULL ||
	    licli_client_new == NULL)
	{
		lisys_error_set (EINVAL, "invalid client library");
		lisys_error_report ();
		lisys_module_free (module);
		return 1;
	}

	/* Create and run the client. */
	client = licli_client_new (name);
	if (client == NULL)
	{
		lisys_module_free (module);
		return 1;
	}
	if (!licli_client_main (client))
	{
		licli_client_free (client);
		lisys_module_free (module);
		return 0;
	}
	licli_client_free (client);
	lisys_module_free (module);

	return 1;
}

static int
generator_main (const char* name)
{
	void* generator;
	void (*ligen_generator_free)(void*);
	int (*ligen_generator_main)(void*);
	void* (*ligen_generator_new)(const char*);
	lisysModule* module;

	/* Open client library. */
	module = private_open_program ("lipsofsunagenerator");
	if (module == NULL)
	{
		lisys_error_append ("cannot load generator program");
		lisys_error_report ();
		return 1;
	}

	/* Find used functions. */
	ligen_generator_free = lisys_module_symbol (module, "ligen_generator_free");
	ligen_generator_main = lisys_module_symbol (module, "ligen_generator_main");
	ligen_generator_new = lisys_module_symbol (module, "ligen_generator_new");
	if (ligen_generator_free == NULL ||
	    ligen_generator_main == NULL ||
	    ligen_generator_new == NULL)
	{
		lisys_error_set (EINVAL, "invalid generator library");
		lisys_error_report ();
		lisys_module_free (module);
		return 1;
	}

	/* Create and run the generator. */
	generator = ligen_generator_new (name);
	if (generator == NULL)
	{
		lisys_module_free (module);
		return 1;
	}
	if (!ligen_generator_main (generator))
	{
		ligen_generator_free (generator);
		lisys_module_free (module);
		return 0;
	}
	ligen_generator_free (generator);
	lisys_module_free (module);

	return 1;
}

static int
server_main (const char* name)
{
	void* server;
	void (*lisrv_server_free)(void*);
	int (*lisrv_server_main)(void*);
	void* (*lisrv_server_new)(const char*);
	lisysModule* module;

	/* Open client library. */
	module = private_open_program ("lipsofsunaserver");
	if (module == NULL)
	{
		lisys_error_append ("cannot load server program");
		lisys_error_report ();
		return 1;
	}

	/* Find used functions. */
	lisrv_server_free = lisys_module_symbol (module, "lisrv_server_free");
	lisrv_server_main = lisys_module_symbol (module, "lisrv_server_main");
	lisrv_server_new = lisys_module_symbol (module, "lisrv_server_new");
	if (lisrv_server_free == NULL ||
	    lisrv_server_main == NULL ||
	    lisrv_server_new == NULL)
	{
		lisys_error_set (EINVAL, "invalid server library");
		lisys_error_report ();
		lisys_module_free (module);
		return 1;
	}

	/* Create and run the server. */
	server = lisrv_server_new (name);
	if (server == NULL)
	{
		lisys_module_free (module);
		return 1;
	}
	if (!lisrv_server_main (server))
	{
		lisrv_server_free (server);
		lisys_module_free (module);
		return 1;
	}
	lisrv_server_free (server);
	lisys_module_free (module);

	return 0;
}

static int
viewer_main (const char* name,
             const char* model)
{
	void* viewer;
	void (*livie_viewer_free)(void*);
	int (*livie_viewer_main)(void*);
	void* (*livie_viewer_new)(const char*, const char*);
	lisysModule* module;

	/* Open viewer library. */
	module = private_open_program ("lipsofsunaviewer");
	if (module == NULL)
	{
		lisys_error_append ("cannot load viewer program");
		lisys_error_report ();
		return 1;
	}

	/* Find used functions. */
	livie_viewer_free = lisys_module_symbol (module, "livie_viewer_free");
	livie_viewer_main = lisys_module_symbol (module, "livie_viewer_main");
	livie_viewer_new = lisys_module_symbol (module, "livie_viewer_new");
	if (livie_viewer_free == NULL ||
	    livie_viewer_main == NULL ||
	    livie_viewer_new == NULL)
	{
		lisys_error_set (EINVAL, "invalid viewer library");
		lisys_error_report ();
		lisys_module_free (module);
		return 1;
	}

	/* Create and run the viewer. */
	viewer = livie_viewer_new (name, model);
	if (viewer == NULL)
	{
		lisys_module_free (module);
		return 1;
	}
	if (!livie_viewer_main (viewer))
	{
		livie_viewer_free (viewer);
		lisys_module_free (module);
		return 0;
	}
	livie_viewer_free (viewer);
	lisys_module_free (module);

	return 1;
}

/** @} */
/** @} */
