#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include "minishell.h"

static int
get_status(int wstatus)
{
	if (WIFEXITED(wstatus))
		return (WEXITSTATUS(wstatus));
	if (WIFSIGNALED(wstatus))
		return (128 + WTERMSIG(wstatus));
	return (1);
}

static void
move_fd(int fd, int target)
{
	if (dup2(fd, target) == -1) {
		perror("minishell: dup2");
		_exit(1);
	}
	close(fd);
}

/* in_fd : sortie du pipe précédent ; out : pipe vers la commande suivante */
static void
child_run(t_shell *shell, t_cmd *cmd, int in_fd, int *out)
{
	int status;

	reset_signals();
	if (in_fd != -1)
		move_fd(in_fd, STDIN_FILENO);
	if (out != NULL) {
		close(out[0]);
		move_fd(out[1], STDOUT_FILENO);
	}
	if (apply_redirs(cmd->redirs) == -1)
		_exit(1);
	if (cmd->args[0] == NULL)
		_exit(0);
	if (is_builtin(cmd->args[0])) {
		shell->interactive = 0; /* pas de message exit dans un fils */
		status = run_builtin(shell, cmd->args);
		fflush(stdout);
		_exit(status);
	}
	execvp(cmd->args[0], cmd->args);
	if (errno == ENOENT) {
		fprintf(stderr, "minishell: %s: command not found\n",
		    cmd->args[0]);
		_exit(127);
	}
	fprintf(stderr, "minishell: %s: %s\n", cmd->args[0], strerror(errno));
	_exit(126);
}

static int
launch(t_shell *shell, t_cmd *cmd, int last, int *in_fd, pid_t *pid)
{
	int fds[2];
	int *out;

	out = NULL;
	if (!last) {
		if (pipe(fds) == -1) {
			perror("minishell: pipe");
			return (-1);
		}
		out = fds;
	}
	*pid = fork();
	if (*pid == -1) {
		perror("minishell: fork");
		if (out != NULL) {
			close(out[0]);
			close(out[1]);
		}
		return (-1);
	}
	if (*pid == 0)
		child_run(shell, cmd, *in_fd, out);
	if (*in_fd != -1)
		close(*in_fd);
	*in_fd = -1;
	if (out != NULL) {
		close(out[1]);
		*in_fd = out[0];
	}
	return (0);
}

/* Le code de retour est celui de la dernière commande, comme bash */
static int
wait_children(pid_t *pids, size_t launched, size_t count)
{
	size_t i;
	int wstatus;
	int status;

	status = 1;
	i = 0;
	while (i < launched) {
		if (waitpid(pids[i], &wstatus, 0) == -1)
			perror("minishell: waitpid");
		else if (i == count - 1)
			status = get_status(wstatus);
		i++;
	}
	return (status);
}

int
exec_pipeline(t_shell *shell, t_pipeline *pl)
{
	pid_t *pids;
	int in_fd;
	size_t i;
	int status;

	pids = malloc(pl->count * sizeof(pid_t));
	if (pids == NULL) {
		perror("minishell");
		return (1);
	}
	g_child_running = 1;
	in_fd = -1;
	i = 0;
	while (i < pl->count) {
		if (launch(shell, &pl->cmds[i], i == pl->count - 1, &in_fd,
			&pids[i]) == -1)
			break;
		i++;
	}
	if (in_fd != -1)
		close(in_fd);
	status = wait_children(pids, i, pl->count);
	g_child_running = 0;
	free(pids);
	return (status);
}
