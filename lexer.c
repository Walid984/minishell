#include <stdlib.h>
#include <string.h>
#include "minishell.h"

#define SPACES " \t\n"
#define OPERATORS "<>"

int
is_operator(const char *token)
{
	return (strchr(OPERATORS, token[0]) != NULL);
}

static size_t
token_len(const char *s)
{
	size_t len;

	if (s[0] == '>' && s[1] == '>')
		return (2);
	if (strchr(OPERATORS, s[0]) != NULL)
		return (1);
	len = 0;
	while (s[len] != '\0' && strchr(SPACES OPERATORS, s[len]) == NULL)
		len++;
	return (len);
}

char **
tokenize(const char *line)
{
	char **tokens;
	char *token;
	size_t count;
	size_t len;

	tokens = calloc(1, sizeof(char *));
	if (tokens == NULL)
		return (NULL);
	count = 0;
	while (*line != '\0') {
		if (strchr(SPACES, *line) != NULL) {
			line++;
			continue;
		}
		len = token_len(line);
		token = strndup(line, len);
		if (token == NULL || push_str(&tokens, &count, token) == -1) {
			free(token);
			free_strs(tokens);
			return (NULL);
		}
		line += len;
	}
	return (tokens);
}
