#include <stdlib.h>
#include <string.h>
#include "minishell.h"

#define DELIMS " \t\n"

char **
split_line(char *line)
{
	char **args;
	char **tmp;
	char *token;
	size_t size;
	size_t i;

	size = 8;
	i = 0;
	args = malloc(size * sizeof(char *));
	if (args == NULL)
		return (NULL);
	token = strtok(line, DELIMS);
	while (token != NULL) {
		if (i + 1 >= size) {
			size *= 2;
			tmp = realloc(args, size * sizeof(char *));
			if (tmp == NULL) {
				free(args);
				return (NULL);
			}
			args = tmp;
		}
		args[i++] = token;
		token = strtok(NULL, DELIMS);
	}
	args[i] = NULL;
	return (args);
}
