/*
 * File: builtins1.c
 * Auth: Webster Mbangani
 *       Bukola Sunday
 */

#include "shell.h"
int (*get_builtin(char *command))(char **argums, char **fronter);
int shellby_exit(char **argums, char **fronter);
int shellby_cd(char **argums, char __attribute__((__unused__)) **fronter);
int shellby_help(char **argums, char __attribute__((__unused__)) **fronter);

/**
 * get_builtin - Matches a command with a corresponding
 *               shellby builtin function.
 * @command: Matching command.
 *
 * Return: A function pointer to the corresponding builtin.
 */
int (*get_builtin(char *command))(char **argums, char **fronter)
{
	builtin_t funcs[] = {
		{ "exit", shellby_exit },
		{ "env", shellby_env },
		{ "setenv", shellby_setenv },
		{ "unsetenv", shellby_unsetenv },
		{ "cd", shellby_cd },
		{ "alias", shellby_alias },
		{ "help", shellby_help },
		{ NULL, NULL }
	};
	int i;

	for (i = 0; funcs[i].name; i++)
	{
		if (_strcmp(funcs[i].name, command) == 0)
			break;
	}
	return (funcs[i].f);
}

/**
 * shellby_exit - Causes normal process termination
 *                for the shellby shell.
 * @argums: An array of arguments containing the exit value.
 * @fronter: A double pointer to the beginning of argums.
 *
 * Return: If there are no arguments - -3.
 *         If the given exit value is invalid - 2.
 *         O/w - exits with the given status value.
 *
 * Description: Upon returning -3, the program exits back in the main function.
 */
int shellby_exit(char **argums, char **fronter)
{
	int i, len_of_int = 10;
	unsigned int num = 0, max = 1 << (sizeof(int) * 8 - 1);

	if (argums[0])
	{
		if (argums[0][0] == '+')
		{
			i = 1;
			len_of_int++;
		}
		for (; argums[0][i]; i++)
		{
			if (i <= len_of_int && argums[0][i] >= '0' && argums[0][i] <= '9')
				num = (num * 10) + (argums[0][i] - '0');
			else
				return (create_error(--argums, 2));
		}
	}
	else
	{
		return (-3);
	}
	if (num > max - 1)
		return (create_error(--argums, 2));
	argums -= 1;
	free_argums(argums, fronter);
	free_env();
	free_alias_list(aliases);
	exit(num);
}

/**
 * shellby_cd - Changes the current directory of the shellby process.
 * @argums: An array of arguments.
 * @fronter: A double pointer to the beginning of argums.
 *
 * Return: If the given string is not a directory - 2.
 *         If an error occurs - -1.
 *         Otherwise - 0.
 */
int shellby_cd(char **argums, char __attribute__((__unused__)) **fronter)
{
	char **dir_info, *new_line = "\n";
	char *oldpwd = NULL, *pwd = NULL;
	struct stat dir;

	oldpwd = getcwd(oldpwd, 0);
	if (!oldpwd)
		return (-1);

	if (argums[0])
	{
		if (*(argums[0]) == '-' || _strcmp(argums[0], "--") == 0)
		{
			if ((argums[0][1] == '-' && argums[0][2] == '\0') ||
					argums[0][1] == '\0')
			{
				if (_getenv("OLDPWD") != NULL)
					(chdir(*_getenv("OLDPWD") + 7));
			}
			else
			{
				free(oldpwd);
				return (create_error(argums, 2));
			}
		}
		else
		{
			if (stat(argums[0], &dir) == 0 && S_ISDIR(dir.st_mode)
					&& ((dir.st_mode & S_IXUSR) != 0))
				chdir(argums[0]);
			else
			{
				free(oldpwd);
				return (create_error(argums, 2));
			}
		}
	}
	else
	{
		if (_getenv("HOME") != NULL)
			chdir(*(_getenv("HOME")) + 5);
	}

	pwd = getcwd(pwd, 0);
	if (!pwd)
		return (-1);

	dir_info = malloc(sizeof(char *) * 2);
	if (!dir_info)
		return (-1);

	dir_info[0] = "OLDPWD";
	dir_info[1] = oldpwd;
	if (shellby_setenv(dir_info, dir_info) == -1)
		return (-1);

	dir_info[0] = "PWD";
	dir_info[1] = pwd;
	if (shellby_setenv(dir_info, dir_info) == -1)
		return (-1);
	if (argums[0] && argums[0][0] == '-' && argums[0][1] != '-')
	{
		write(STDOUT_FILENO, pwd, _strlen(pwd));
		write(STDOUT_FILENO, new_line, 1);
	}
	free(oldpwd);
	free(pwd);
	free(dir_info);
	return (0);
}

/**
 * shellby_help - Displays information about shellby builtin commands.
 * @argums: An array of arguments.
 * @fronter: A pointer to the beginning of argums.
 *
 * Return: If an error occurs - -1.
 *         Otherwise - 0.
 */
int shellby_help(char **argums, char __attribute__((__unused__)) **fronter)
{
	if (!argums[0])
		help_all();
	else if (_strcmp(argums[0], "alias") == 0)
		help_alias();
	else if (_strcmp(argums[0], "cd") == 0)
		help_cd();
	else if (_strcmp(argums[0], "exit") == 0)
		help_exit();
	else if (_strcmp(argums[0], "env") == 0)
		help_env();
	else if (_strcmp(argums[0], "setenv") == 0)
		help_setenv();
	else if (_strcmp(argums[0], "unsetenv") == 0)
		help_unsetenv();
	else if (_strcmp(argums[0], "help") == 0)
		help_help();
	else
		write(STDERR_FILENO, name, _strlen(name));

	return (0);
}
