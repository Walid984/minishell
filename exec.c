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
child_exec(t_cmd *cmd)
{
	reset_signals();
	if (apply_redirs(cmd->redirs) == -1)
		_exit(1);
	if (cmd->args[0] == NULL)
		_exit(0);
	execvp(cmd->args[0], cmd->args);
	if (errno == ENOENT) {
		fprintf(stderr, "minishell: %s: command not found\n",
		    cmd->args[0]);
		_exit(127);
	}
	fprintf(stderr, "minishell: %s: %s\n", cmd->args[0], strerror(errno));
	_exit(126);
}

int
exec_command(t_cmd *cmd)
{
	pid_t pid;
	int wstatus;
	int ret;

	g_child_running = 1;
	pid = fork();
	if (pid == -1) {
		g_child_running = 0;
		perror("minishell: fork");
		return (1);
	}
	if (pid == 0)
		child_exec(cmd);
	ret = waitpid(pid, &wstatus, 0);
	g_child_running = 0;
	if (ret == -1) {
		perror("minishell: waitpid");
		return (1);
	}
	return (get_status(wstatus));
}
