#ifndef MINISHELL_H
#define MINISHELL_H

#include <signal.h>

#define PROMPT "minishell$ "

typedef struct s_shell {
	int status;
	int running;
} t_shell;

extern volatile sig_atomic_t g_child_running;

char **split_line(char *line);
int exec_command(char **args);
int is_builtin(const char *name);
int run_builtin(t_shell *shell, char **args);
void setup_signals(void);
void reset_signals(void);

#endif
