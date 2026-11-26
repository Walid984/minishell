#include <stdlib.h>
#include "minishell.h"

int
push_str(char ***arr, size_t *len, char *str)
{
	char **tmp;

	tmp = realloc(*arr, (*len + 2) * sizeof(char *));
	if (tmp == NULL)
		return (-1);
	tmp[(*len)++] = str;
	tmp[*len] = NULL;
	*arr = tmp;
	return (0);
}

void
free_strs(char **strs)
{
	size_t i;

	if (strs == NULL)
		return;
	i = 0;
	while (strs[i] != NULL)
		free(strs[i++]);
	free(strs);
}
