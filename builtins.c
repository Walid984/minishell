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

/* Builtin lancé dans le shell : on sauvegarde stdin/stdout autour */
int
run_builtin_redir(t_shell *shell, t_cmd *cmd)
{
	int saved_in;
	int saved_out;
	int status;

	saved_in = dup(STDIN_FILENO);
	saved_out = dup(STDOUT_FILENO);
	if (saved_in == -1 || saved_out == -1) {
		perror("minishell: dup");
		if (saved_in != -1)
			close(saved_in);
		if (saved_out != -1)
			close(saved_out);
		return (1);
	}
	status = 1;
	if (apply_redirs(cmd->redirs) == 0)
		status = run_builtin(shell, cmd->args);
	fflush(stdout);
	dup2(saved_in, STDIN_FILENO);
	dup2(saved_out, STDOUT_FILENO);
	close(saved_in);
	close(saved_out);
	return (status);
}
