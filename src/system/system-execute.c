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
 * \addtogroup lisys System
 * @{
 * \addtogroup lisysExec Execute
 * @{
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif
#include "system-error.h"
#include "system-execute.h"

static int
private_execvp_detach (const char*        prog,
                       const char* const* args)
{
#if defined HAVE_FORK

	int status;
	pid_t pid;

	/* Create child and wait for it to exit. */
	pid = fork ();
	if (pid == -1)
	{
		lisys_error_set (LI_ERROR_UNKNOWN, "cannot fork");
		return 0;
	}
	if (pid != 0)
	{
		waitpid (pid, &status, 0);
		if (!WIFEXITED (status) || WEXITSTATUS (status))
		{
			lisys_error_set (LI_ERROR_UNKNOWN, "client process did not exit successfully");
			return 0;
		}
		return 1;
	}

	/* Fork in the child and exit. */
	pid = fork ();
	if (pid == -1)
		_exit (-errno);
	if (pid != 0)
		_exit (0);

	/* Execute program. */
	execvp (prog, (char* const*) args);
	_exit (-errno);

	return 0;

#else

	lisys_error_set (ENOTSUP, NULL);
	return 0;

#endif
}

static int
private_execvp_redir (lisysExecFilter    filter,
                      void*              data,
                      const char*        prog,
                      const char* const* args)
{
#if defined HAVE_FORK

	pid_t pid;
	FILE* input;
	int status;
	int pipefd[2];

	/* Create pipe. */
	if (filter == NULL || pipe (pipefd) == -1)
	{
		pipefd[0] = -1;
		pipefd[1] = -1;
	}

	/* Create child process. */
	pid = fork ();
	if (pid == -1)
	{
		lisys_error_set (errno, "cannot fork");
		return 0;
	}

	/* Let child execute. */
	if (pid == 0)
	{
		/* Redirect output. */
		if (pipefd[0] != -1)
		{
			close (pipefd[0]);
			if (pipefd[1] != 1)
			{
				dup2 (pipefd[1], 1);
				close (pipefd[1]);
			}
		}

		/* Execute program. */
		execvp (prog, (char* const*) args);
		_exit (-errno);
	}

	/* Process child output. */
	if (pipefd[0] != -1)
	{
		input = fdopen (pipefd[0], "r");
		close (pipefd[1]);
		if (input != NULL)
			filter (data, input);
		if (input != NULL)
			fclose (input);
		close (pipefd[0]);
	}

	/* Wait for child to exit. */
	waitpid (pid, &status, 0);
	if (!WIFEXITED (status))
	{
		lisys_error_set (ECANCELED, "child process terminated unexpectedly");
		return 0;
	}
	else if (WEXITSTATUS (status) < 0)
	{
		lisys_error_set (-WEXITSTATUS (status), "child process did not exit successfully");
		return 0;
	}
	else if (WEXITSTATUS (status) > 0)
	{
		lisys_error_set (ECANCELED, "child process did not exit successfully");
		return 0;
	}

	return 1;

#elif defined _WIN32

	int l;
	int len = 0;
	char* tmp;
	char* cmd = NULL;
	const char* const* arg;
	STARTUPINFO startup_info;
	PROCESS_INFORMATION process_info;

	/* Format command. */
	/* FIXME: Spaces in arguments will break. */
	for (arg = args ; *arg != NULL ; arg++)
	{
		if (arg == args)
			continue;
		l = strlen (*arg);
		tmp = realloc (cmd, len + l + 2);
		if (tmp == NULL)
		{
			lisys_error_set (ENOMEM, NULL);
			free (cmd);
			return 0;
		}
		cmd = tmp;
		cmd[len] = ' ';
		strncpy (cmd + len + 1, *arg, l);
		len += l + 1;
		cmd[len] = '\0';
	}

	/* Prepare process info. */
	memset (&startup_info, 0, sizeof (startup_info));
	memset (&process_info, 0, sizeof (process_info));
	startup_info.cb = sizeof (startup_info);
	if (filter != NULL)
	{
	//	startup_info.dwFlags = STARTF_USESTDHANDLES;
		//startup_info.hStdOutput = CreatePipe (PHANDLE,PHANDLE,LPSECURITY_ATTRIBUTES,DWORD);
	}

	/* Execute the command. */
	if (!CreateProcess (prog, cmd, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS,
		NULL, NULL, &startup_info, &process_info))
	{
		lisys_error_set (LI_ERROR_UNKNOWN, "cannot start child process");
		free (cmd);
		return 0;
	}
	free (cmd);

	/* TODO: Process child output. */

	/* Wait for child to exit. */
	WaitForSingleObject (process_info.hProcess, INFINITE);
	CloseHandle (process_info.hProcess);
	CloseHandle (process_info.hThread);

	return 1;

#else

	lisys_error_set (ENOTSUP, NULL);
	return 0;

#endif
}

static void
private_redir_file (void* data,
                    FILE* file)
{
}

/*****************************************************************************/

/**
 * \brief Executes a command.
 *
 * \param prog Program name.
 * \param ... Command and its parameters terminated by NULL.
 * \return Nonzero on success.
 */
int
lisys_execvl (const char* prog,
                          ...)
{
	int i;
	va_list args;
	const char* argv[32];

	va_start (args, prog);
	for (i = 0 ; i < 32 ; i++)
	{
		argv[i] = va_arg (args, char*);
		if (argv[i] == NULL)
			break;
	}
	va_end (args);
	if (i == 32)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}

	return private_execvp_redir (NULL, NULL, prog, argv);
}

/**
 * \brief Executes a command and detaches it from the current process.
 *
 * Performs a double fork and exec in the second child on POSIX systems to
 * ensure that the executed program is detached from the current process.
 *
 * \param prog Program name.
 * \param ... Command and its parameters terminated by NULL.
 * \return Nonzero on success.
 */
int
lisys_execvl_detach (const char* prog,
                                 ...)
{
	int i;
	va_list args;
	const char* argv[32];

	va_start (args, prog);
	for (i = 0 ; i < 32 ; i++)
	{
		argv[i] = va_arg (args, char*);
		if (argv[i] == NULL)
			break;
	}
	va_end (args);
	if (i == 32)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}

	return private_execvp_detach (prog, argv);
}

/**
 * \brief Executes a command and passes its output to a callback.
 *
 * The function redirects the output of the command to a pipe and passes a
 * file pointer encapsulating the pipe to the callback for processing.
 *
 * \param call Callback to process the output.
 * \param data User data pointer that will be passed to the callback.
 * \param prog Program name.
 * \param ... Command and its parameters terminated by NULL.
 * \return Nonzero on success.
 */
int
lisys_execvl_redir_call (lisysExecFilter call,
                         void*           data,
                         const char*     prog,
                                         ...)
{
	int i;
	va_list args;
	const char* argv[32];

	va_start (args, prog);
	for (i = 0 ; i < 32 ; i++)
	{
		argv[i] = va_arg (args, char*);
		if (argv[i] == NULL)
			break;
	}
	va_end (args);
	if (i == 32)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}

	return private_execvp_redir (call, data, prog, argv);
}

/**
 * \brief Executes a command and redirects its output to a file.
 *
 * \param file File where to redirect or NULL.
 * \param prog Program name.
 * \param ... Command and its parameters terminated by NULL.
 * \return Nonzero on success.
 */
int
lisys_execvl_redir_file (FILE*       file,
                         const char* prog,
                                     ...)
{
	int i;
	va_list args;
	const char* argv[32];

	va_start (args, prog);
	for (i = 0 ; i < 32 ; i++)
	{
		argv[i] = va_arg (args, char*);
		if (argv[i] == NULL)
			break;
	}
	va_end (args);
	if (i == 32)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}

	return private_execvp_redir (private_redir_file, file, prog, argv);
}

/**
 * \brief Executes a command and redirects its output to a file specified by path.
 *
 * \param path Path to file where to redirect.
 * \param prog Program name.
 * \param ... Command and its parameters terminated by NULL.
 * \return Nonzero on success.
 */
int
lisys_execvl_redir_path (const char* path,
                         const char* prog,
                                     ...)
{
	int i;
	va_list args;
	const char* argv[32];

	va_start (args, prog);
	for (i = 0 ; i < 32 ; i++)
	{
		argv[i] = va_arg (args, char*);
		if (argv[i] == NULL)
			break;
	}
	va_end (args);
	if (i == 32)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}

	return lisys_execvp_redir_path (path, prog, argv);
}

/**
 * \brief Executes a command.
 *
 * \param prog Program name.
 * \param args Command and its parameters terminated by NULL.
 * \return Nonzero on success.
 */
int
lisys_execvp (const char*        prog,
              const char* const* args)
{
	return private_execvp_redir (NULL, NULL, prog, args);
}

/**
 * \brief Executes a command and detaches it from the current process.
 *
 * Performs a double fork and exec in the second child on POSIX systems to
 * ensure that the executed program is detached from the current process.
 *
 * \param prog Program name.
 * \param args Command and its parameters terminated by NULL.
 * \return Nonzero on success.
 */
int
lisys_execvp_detach (const char*        prog,
                     const char* const* args)
{
	return private_execvp_detach (prog, args);
}

/**
 * \brief Executes a command and passes its output to a callback.
 *
 * The function redirects the output of the command to a pipe and passes a
 * file pointer encapsulating the pipe to the callback for processing.
 *
 * \param call Callback to process the output.
 * \param data User data pointer that will be passed to the callback.
 * \param prog Program name.
 * \param args Command and its parameters terminated by NULL.
 * \return Nonzero on success.
 */
int
lisys_execvp_redir_call (lisysExecFilter    call,
                         void*              data,
                         const char*        prog,
                         const char* const* args)
{
	return private_execvp_redir (call, data, prog, args);
}

/**
 * \brief Executes a command and redirects its output to a file.
 *
 * \param file File where to redirect or NULL.
 * \param prog Program name.
 * \param args Command and its parameters terminated by NULL.
 * \return Nonzero on success.
 */
int
lisys_execvp_redir_file (FILE*              file,
                         const char*        prog,
                         const char* const* args)
{
	return private_execvp_redir (private_redir_file, file, prog, args);
}

/**
 * \brief Executes a command and redirects its output to a file specified by path.
 *
 * \param path Path to file where to redirect.
 * \param prog Program name.
 * \param args Command and its parameters terminated by NULL.
 * \return Nonzero on success.
 */
int
lisys_execvp_redir_path (const char*        path,
                         const char*        prog,
                         const char* const* args)
{
	int ret;
	FILE* file;

	file = fopen (path, "w");
	if (file == NULL)
	{
		lisys_error_set (errno, NULL);
		return 0;
	}
	ret = private_execvp_redir (private_redir_file, file, prog, args);
	fclose (file);

	return ret;
}

/** @} */
/** @} */
