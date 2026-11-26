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
run_cmd(t_shell *shell, t_cmd *cmd)
{
	if (cmd->args[0] != NULL && is_builtin(cmd->args[0]))
		shell->status = run_builtin_redir(shell, cmd);
	else
		shell->status = exec_command(cmd);
}

static void
run_line(t_shell *shell, char *line)
{
	char **tokens;
	t_cmd cmd;
	int ret;

	tokens = tokenize(line);
	if (tokens == NULL) {
		perror("minishell");
		shell->status = 1;
		return;
	}
	if (tokens[0] != NULL) {
		ret = parse_cmd(tokens, &cmd);
		if (ret != 0) {
			shell->status = ret;
		} else {
			run_cmd(shell, &cmd);
			free_cmd(&cmd);
		}
	}
	free_strs(tokens);
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
	if (isatty(STDIN_FILENO))
		setup_signals();
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
