#ifndef MINISHELL_H
#define MINISHELL_H

#define PROMPT "minishell$ "

typedef struct s_shell {
	int status;
	int running;
} t_shell;

char **split_line(char *line);
int exec_command(char **args);

#endif
