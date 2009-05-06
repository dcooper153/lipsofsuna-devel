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

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "system-error.h"
#include "system-execute.h"

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

	return lisys_execvp (prog, argv);
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

	return lisys_execvp_detach (prog, argv);
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

	return lisys_execvp_redir_call (call, data, prog, argv);
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

	return lisys_execvp_redir_file (file, prog, argv);
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
	pid_t pid;
	int status;

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
		execvp (prog, (char* const*) args);
		_exit (-errno);
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
	pid_t pid;
	FILE* input;
	int status;
	int pipefd[2];

	/* Create pipe. */
	if (pipe (pipefd) == -1)
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
			call (data, input);
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
	pid_t pid;
	size_t len;
	FILE* input;
	int status;
	int pipefd[2];
	char buffer[1024];

	/* Create pipe. */
	if (pipe (pipefd) == -1)
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
		{
			while (!feof (input) && !ferror (input))
			{
				len = fread (buffer, 1, sizeof (buffer), input);
				if (len > 0 && file != NULL)
					len = fwrite (buffer, 1, len, file);
			}
		}
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
	ret = lisys_execvp_redir_file (file, prog, args);
	fclose (file);

	return ret;
}

/** @} */
/** @} */
