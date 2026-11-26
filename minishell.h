#ifndef MINISHELL_H
#define MINISHELL_H

#include <signal.h>
#include <stddef.h>

#define PROMPT "minishell$ "

typedef struct s_shell {
	int status;
	int running;
} t_shell;

/* args : mots de la commande ; redirs : paires opérateur/fichier */
typedef struct s_cmd {
	char **args;
	char **redirs;
} t_cmd;

extern volatile sig_atomic_t g_child_running;

char **tokenize(const char *line);
int is_operator(const char *token);
int push_str(char ***arr, size_t *len, char *str);
void free_strs(char **strs);
int parse_cmd(char **tokens, t_cmd *cmd);
void free_cmd(t_cmd *cmd);
int apply_redirs(char **redirs);
int exec_command(t_cmd *cmd);
int is_builtin(const char *name);
int run_builtin(t_shell *shell, char **args);
int run_builtin_redir(t_shell *shell, t_cmd *cmd);
void setup_signals(void);
void reset_signals(void);

#endif
