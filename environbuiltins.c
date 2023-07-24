/*
 * File: environbuiltins.c
 * Auth: Webster Mbangani
 *       Bukola Sunday
 */

#include "shell.h"

int shellby_env(char **argums, char __attribute__((__unused__)) **fronter);
int shellby_setenv(char **argums, char __attribute__((__unused__)) **fronter);
int shellby_unsetenv(char **argums, char __attribute__((__unused__)) **fronter);

/**
 * shellby_env - Prints the current environment.
 * @argums: An array of arguments passed to the shell.
 * @fronter: A double pointer to the beginning of argums.
 *
 * Return: If an error occurs - -1.
 *	   Otherwise - 0.
 *
 * Description: Prints one variable per line in the
 *              format 'variable'='value'.
 */
int shellby_env(char **argums, char __attribute__((__unused__)) **fronter)
{
	int index;
	char nc = '\n';

	if (!environ)
		return (-1);

	for (index = 0; environ[index]; index++)
	{
		write(STDOUT_FILENO, environ[index], _strlen(environ[index]));
		write(STDOUT_FILENO, &nc, 1);
	}

	(void)argums;
	return (0);
}

/**
 * shellby_setenv - Changes or adds an environmental variable to the PATH.
 * @argums: An array of arguments passed to the shell.
 * @fronter: A double pointer to the beginning of argums.
 * Description: argums[1] is the name of the new or existing PATH variable.
 *              argums[2] is the value to set the new or changed variable to.
 *
 * Return: If an error occurs - -1.
 *         Otherwise - 0.
 */
int shellby_setenv(char **argums, char __attribute__((__unused__)) **fronter)
{
	char **env_var = NULL, **new_environ, *new_value;
	size_t size;
	int index;

	if (!argums[0] || !argums[1])
		return (create_error(argums, -1));

	new_value = malloc(_strlen(argums[0]) + 1 + _strlen(argums[1]) + 1);
	if (!new_value)
		return (create_error(argums, -1));
	_strcpy(new_value, argums[0]);
	_strcat(new_value, "=");
	_strcat(new_value, argums[1]);

	env_var = _getenv(argums[0]);
	if (env_var)
	{
		free(*env_var);
		*env_var = new_value;
		return (0);
	}
	for (size = 0; environ[size]; size++)
		;

	new_environ = malloc(sizeof(char *) * (size + 2));
	if (!new_environ)
	{
		free(new_value);
		return (create_error(argums, -1));
	}

	for (index = 0; environ[index]; index++)
		new_environ[index] = environ[index];

	free(environ);
	environ = new_environ;
	environ[index] = new_value;
	environ[index + 1] = NULL;

	return (0);
}

/**
 * shellby_unsetenv - Deletes an environmental variable from the PATH.
 * @argums: An array of arguments passed to the shell.
 * @fronter: A double pointer to the beginning of argums.
 * Description: argums[1] is the PATH variable to remove.
 *
 * Return: If an error occurs - -1.
 *         Otherwise - 0.
 */
int shellby_unsetenv(char **argums, char __attribute__((__unused__)) **fronter)
{
	char **env_var, **new_environ;
	size_t size;
	int index, index2;

	if (!argums[0])
		return (create_error(argums, -1));
	env_var = _getenv(argums[0]);
	if (!env_var)
		return (0);

	for (size = 0; environ[size]; size++)
		;

	new_environ = malloc(sizeof(char *) * size);
	if (!new_environ)
		return (create_error(argums, -1));

	for (index = 0, index2 = 0; environ[index]; index++)
	{
		if (*env_var == environ[index])
		{
			free(*env_var);
			continue;
		}
		new_environ[index2] = environ[index];
		index2++;
	}
	free(environ);
	environ = new_environ;
	environ[size - 1] = NULL;

	return (0);
}
