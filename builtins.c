#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "minishell.h"

int
is_builtin(const char *name)
{
	return (strcmp(name, "cd") == 0 || strcmp(name, "exit") == 0);
}

static int
builtin_cd(char **args)
{
	const char *path;

	if (args[1] != NULL && args[2] != NULL) {
		fprintf(stderr, "minishell: cd: too many arguments\n");
		return (1);
	}
	path = args[1];
	if (path == NULL) {
		path = getenv("HOME");
		if (path == NULL) {
			fprintf(stderr, "minishell: cd: HOME not set\n");
			return (1);
		}
	}
	if (chdir(path) == -1) {
		fprintf(stderr, "minishell: cd: %s: %s\n", path,
		    strerror(errno));
		return (1);
	}
	return (0);
}

static int
builtin_exit(t_shell *shell, char **args)
{
	char *end;
	long code;

	if (isatty(STDIN_FILENO))
		printf("exit\n");
	if (args[1] == NULL) {
		shell->running = 0;
		return (shell->status);
	}
	code = strtol(args[1], &end, 10);
	if (end == args[1] || *end != '\0') {
		fprintf(stderr,
		    "minishell: exit: %s: numeric argument required\n",
		    args[1]);
		shell->running = 0;
		return (2);
	}
	if (args[2] != NULL) {
		fprintf(stderr, "minishell: exit: too many arguments\n");
		return (1);
	}
	shell->running = 0;
	return ((unsigned char)code);
}

int
run_builtin(t_shell *shell, char **args)
{
	if (strcmp(args[0], "cd") == 0)
		return (builtin_cd(args));
	return (builtin_exit(shell, args));
}
