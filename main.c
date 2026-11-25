#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "minishell.h"

static void
print_prompt(void)
{
	if (isatty(STDIN_FILENO)) {
		printf("%s", PROMPT);
		fflush(stdout);
	}
}

static void
run_line(t_shell *shell, char *line)
{
	char **args;

	args = split_line(line);
	if (args == NULL) {
		perror("minishell");
		shell->status = 1;
		return;
	}
	if (args[0] != NULL && is_builtin(args[0]))
		shell->status = run_builtin(shell, args);
	else if (args[0] != NULL)
		shell->status = exec_command(args);
	free(args);
}

int
main(void)
{
	t_shell shell;
	char *line;
	size_t cap;

	shell.status = 0;
	shell.running = 1;
	line = NULL;
	cap = 0;
	while (shell.running) {
		print_prompt();
		if (getline(&line, &cap, stdin) == -1) {
			if (isatty(STDIN_FILENO))
				printf("exit\n");
			break;
		}
		run_line(&shell, line);
	}
	free(line);
	return (shell.status);
}
