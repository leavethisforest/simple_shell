/*
 * File: main.c
 * Auth: Webster Mbangani
 *       Bukola Sunday
 */

#include "shell.h"

void signal_hand(int signaler);
int execute(char **argums, char **fronter);

/**
 * signal_hand - Prints a new prompt upon a signal.
 * @signaler: Signal.
 */
void signal_hand(int signaler)
{
	char *new_prompt = "\n$ ";

	(void)signaler;
	signal(SIGINT, signal_hand);
	write(STDIN_FILENO, new_prompt, 3);
}

/**
 * execute - Executes a command in a child process.
 * @argums: An array of arguments.
 * @fronter: A double pointer to the beginning of argums.
 *
 * Return: If an error occurs - a corresponding error code.
 *         O/w - The exit value of the last executed command.
 */
int execute(char **argums, char **fronter)
{
	pid_t child_pid;
	int status, flag = 0, ret = 0;
	char *command = argums[0];

	if (command[0] != '/' && command[0] != '.')
	{
		flag = 1;
		command = get_location(command);
	}

	if (!command || (access(command, F_OK) == -1))
	{
		if (errno == EACCES)
			ret = (create_error(argums, 126));
		else
			ret = (create_error(argums, 127));
	}
	else
	{
		child_pid = fork();
		if (child_pid == -1)
		{
			if (flag)
				free(command);
			perror("Error child:");
			return (1);
		}
		if (child_pid == 0)
		{
			execve(command, argums, environ);
			if (errno == EACCES)
				ret = (create_error(argums, 126));
			free_env();
			free_argums(argums, fronter);
			free_alias_list(aliases);
			_exit(ret);
		}
		else
		{
			wait(&status);
			ret = WEXITSTATUS(status);
		}
	}
	if (flag)
		free(command);
	return (ret);
}

/**
 * main - Runs a simple UNIX command interpreter.
 * @argcount: Number of arguments supplied to the program.
 * @argv: Array of pointers to the arguments.
 *
 * Return: The return value of the last executed command.
 */
int main(int argcount, char *argv[])
{
	int ret = 0, retn;
	int *exe_ret = &retn;
	char *prompt = "$ ", *new_line = "\n";

	name = argv[0];
	hist = 1;
	aliases = NULL;
	signal(SIGINT, signal_hand);

	*exe_ret = 0;
	environ = _copyenv();
	if (!environ)
		exit(-100);

	if (argcount != 1)
	{
		ret = proc_file_commands(argvar[1], exe_ret);
		free_env();
		free_alias_list(aliases);
		return (*exe_ret);
	}

	if (!isatty(STDIN_FILENO))
	{
		while (ret != END_OF_FILE && ret != EXIT)
			ret = handle_argums(exe_ret);
		free_env();
		free_alias_list(aliases);
		return (*exe_ret);
	}

	while (1)
	{
		write(STDOUT_FILENO, prompt, 2);
		ret = handle_argums(exe_ret);
		if (ret == END_OF_FILE || ret == EXIT)
		{
			if (ret == END_OF_FILE)
				write(STDOUT_FILENO, new_line, 1);
			free_env();
			free_alias_list(aliases);
			exit(*exe_ret);
		}
	}

	free_env();
	free_alias_list(aliases);
	return (*exe_ret);
}