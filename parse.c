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
syntax_error(const char *token)
{
	if (token == NULL)
		token = "newline";
	fprintf(stderr, "minishell: syntax error near unexpected token `%s'\n",
	    token);
	return (2);
}

static int
alloc_error(void)
{
	perror("minishell");
	return (1);
}

/* Lit une commande jusqu'au prochain | ou la fin de la ligne */
static int
parse_cmd(char **tokens, size_t *i, t_cmd *cmd)
{
	size_t nargs;
	size_t nredirs;
	int err;

	cmd->args = calloc(1, sizeof(char *));
	cmd->redirs = calloc(1, sizeof(char *));
	if (cmd->args == NULL || cmd->redirs == NULL)
		return (alloc_error());
	nargs = 0;
	nredirs = 0;
	while (tokens[*i] != NULL && strcmp(tokens[*i], "|") != 0) {
		if (!is_redir(tokens[*i])) {
			err = push_str(&cmd->args, &nargs, tokens[(*i)++]);
		} else if (tokens[*i + 1] == NULL ||
		    is_operator(tokens[*i + 1])) {
			return (syntax_error(tokens[*i + 1]));
		} else {
			err = push_str(&cmd->redirs, &nredirs, tokens[*i]);
			if (err == 0)
				err = push_str(&cmd->redirs, &nredirs,
				    tokens[*i + 1]);
			*i += 2;
		}
		if (err != 0)
			return (alloc_error());
	}
	if (nargs == 0 && nredirs == 0)
		return (syntax_error(tokens[*i]));
	return (0);
}

static int
add_cmd(t_pipeline *pl)
{
	t_cmd *tmp;

	tmp = realloc(pl->cmds, (pl->count + 1) * sizeof(t_cmd));
	if (tmp == NULL)
		return (alloc_error());
	pl->cmds = tmp;
	pl->cmds[pl->count].args = NULL;
	pl->cmds[pl->count].redirs = NULL;
	pl->count++;
	return (0);
}

int
parse_line(char **tokens, t_pipeline *pl)
{
	size_t i;
	int ret;

	pl->cmds = NULL;
	pl->count = 0;
	i = 0;
	ret = add_cmd(pl);
	while (ret == 0) {
		ret = parse_cmd(tokens, &i, &pl->cmds[pl->count - 1]);
		if (ret != 0 || tokens[i] == NULL)
			break;
		i++;
		ret = add_cmd(pl);
	}
	if (ret != 0)
		free_pipeline(pl);
	return (ret);
}

void
free_pipeline(t_pipeline *pl)
{
	size_t i;

	i = 0;
	while (i < pl->count) {
		free(pl->cmds[i].args);
		free(pl->cmds[i].redirs);
		i++;
	}
	free(pl->cmds);
	pl->cmds = NULL;
	pl->count = 0;
}
