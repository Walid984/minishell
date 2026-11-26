#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minishell.h"

static int
is_redir(const char *token)
{
	return (strcmp(token, "<") == 0 || strcmp(token, ">") == 0 ||
	    strcmp(token, ">>") == 0);
}

static int
syntax_error(t_cmd *cmd, const char *token)
{
	free_cmd(cmd);
	if (token == NULL)
		token = "newline";
	fprintf(stderr, "minishell: syntax error near unexpected token `%s'\n",
	    token);
	return (2);
}

static int
alloc_error(t_cmd *cmd)
{
	free_cmd(cmd);
	perror("minishell");
	return (1);
}

int
parse_cmd(char **tokens, t_cmd *cmd)
{
	size_t i;
	size_t nargs;
	size_t nredirs;
	int err;

	cmd->args = calloc(1, sizeof(char *));
	cmd->redirs = calloc(1, sizeof(char *));
	if (cmd->args == NULL || cmd->redirs == NULL)
		return (alloc_error(cmd));
	i = 0;
	nargs = 0;
	nredirs = 0;
	while (tokens[i] != NULL) {
		if (!is_redir(tokens[i])) {
			err = push_str(&cmd->args, &nargs, tokens[i++]);
		} else if (tokens[i + 1] == NULL ||
		    is_operator(tokens[i + 1])) {
			return (syntax_error(cmd, tokens[i + 1]));
		} else {
			err = push_str(&cmd->redirs, &nredirs, tokens[i]);
			if (err == 0)
				err = push_str(&cmd->redirs, &nredirs,
				    tokens[i + 1]);
			i += 2;
		}
		if (err != 0)
			return (alloc_error(cmd));
	}
	return (0);
}

void
free_cmd(t_cmd *cmd)
{
	free(cmd->args);
	free(cmd->redirs);
	cmd->args = NULL;
	cmd->redirs = NULL;
}
