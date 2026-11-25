#include <signal.h>
#include <unistd.h>
#include "minishell.h"

volatile sig_atomic_t g_child_running = 0;

static void
handle_sigint(int sig)
{
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	if (!g_child_running)
		write(STDOUT_FILENO, PROMPT, sizeof(PROMPT) - 1);
}

void
setup_signals(void)
{
	struct sigaction sa;

	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}

void
reset_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}
