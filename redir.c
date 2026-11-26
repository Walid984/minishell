#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "minishell.h"

static int
open_redir(const char *op, const char *file)
{
	if (strcmp(op, "<") == 0)
		return (open(file, O_RDONLY));
	if (strcmp(op, ">>") == 0)
		return (open(file, O_WRONLY | O_CREAT | O_APPEND, 0644));
	return (open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644));
}

/* Applique les redirections dans l'ordre, comme bash */
int
apply_redirs(char **redirs)
{
	size_t i;
	int fd;
	int target;

	i = 0;
	while (redirs[i] != NULL) {
		fd = open_redir(redirs[i], redirs[i + 1]);
		if (fd == -1) {
			fprintf(stderr, "minishell: %s: %s\n", redirs[i + 1],
			    strerror(errno));
			return (-1);
		}
		target = STDOUT_FILENO;
		if (redirs[i][0] == '<')
			target = STDIN_FILENO;
		if (dup2(fd, target) == -1) {
			perror("minishell: dup2");
			close(fd);
			return (-1);
		}
		close(fd);
		i += 2;
	}
	return (0);
}
