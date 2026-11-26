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

/* Un builtin seul tourne dans le shell (cd doit changer son répertoire) */
static void
run_pipeline(t_shell *shell, t_pipeline *pl)
{
	t_cmd *first;

	first = &pl->cmds[0];
	if (pl->count == 1 && first->args[0] != NULL &&
	    is_builtin(first->args[0]))
		shell->status = run_builtin_redir(shell, first);
	else
		shell->status = exec_pipeline(shell, pl);
}

static void
run_line(t_shell *shell, char *line)
{
	char **tokens;
	t_pipeline pl;
	int ret;

	tokens = tokenize(line);
	if (tokens == NULL) {
		perror("minishell");
		shell->status = 1;
		return;
	}
	if (tokens[0] != NULL) {
		ret = parse_line(tokens, &pl);
		if (ret != 0) {
			shell->status = ret;
		} else {
			run_pipeline(shell, &pl);
			free_pipeline(&pl);
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
	shell.interactive = isatty(STDIN_FILENO);
	line = NULL;
	cap = 0;
	if (shell.interactive)
		setup_signals();
	while (shell.running) {
		print_prompt();
		if (getline(&line, &cap, stdin) == -1) {
			if (shell.interactive)
				fprintf(stderr, "exit\n");
			break;
		}
		run_line(&shell, line);
	}
	free(line);
	return (shell.status);
}
