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

static int
print_help (const char* exe);

static int
client_main ();

static int
server_main ();

/*****************************************************************************/

int
main (int argc, char** argv)
{
	int i;
	int dedicated = 0;
	const char* name;

	/* Parse arguments. */
	for (i = 1 ; i < argc ; i++)
	{
		if (!strcmp (argv[i], "-h") ||
		    !strcmp (argv[i], "--help"))
			return print_help (argv[0]);
		else if (!strcmp (argv[i], "-d") ||
		         !strcmp (argv[i], "--dedicated"))
			dedicated = 1;
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
	if (dedicated)
		return server_main (name);
	else
		return client_main (name);
}

/*****************************************************************************/

static int
print_help (const char* exe)
{
	printf ("Usage: %s [OPTION] [MODULE]\n\n", exe);
	printf ("  -d, --dedicated  Run as a dedicated server.\n");
	printf ("  -h, --help       Display this help and exit.\n\n");
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
	module = lisys_module_new ("liblipsclient." LISYS_EXTENSION_DLL,
		LISYS_MODULE_FLAG_GLOBAL | LISYS_MODULE_FLAG_LIBDIRS);
	if (module == NULL)
	{
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
		lisys_error_report ();
		lisys_module_free (module);
		return 1;
	}
	if (!licli_client_main (client))
	{
		lisys_error_report ();
		licli_client_free (client);
		lisys_module_free (module);
		return 0;
	}
	licli_client_free (client);
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
	module = lisys_module_new ("liblipsserver." LISYS_EXTENSION_DLL,
		LISYS_MODULE_FLAG_GLOBAL | LISYS_MODULE_FLAG_LIBDIRS);
	if (module == NULL)
	{
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
		lisys_error_set (EINVAL, "invalid client library");
		lisys_error_report ();
		lisys_module_free (module);
		return 1;
	}

	/* Create and run the server. */
	server = lisrv_server_new (name);
	if (server == NULL)
	{
		lisys_error_report ();
		return 1;
	}
	if (!lisrv_server_main (server))
	{
		lisys_error_report ();
		lisrv_server_free (server);
		return 1;
	}
	lisrv_server_free (server);

	return 0;
}

/** @} */
/** @} */
