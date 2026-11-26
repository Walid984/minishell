# minishell

Projet libre en C (3SI4) - Walid Hajjout

Un petit shell en C qui lit des commandes, les exécute et gère les
redirections et les pipes. Le but était de comprendre comment un shell
fonctionne en dessous : `fork`, `execvp`, `waitpid`, `pipe`, `dup2` et les
signaux.

## Compilation

```sh
make          # compile ./minishell
make debug    # recompile avec -g et les sanitizers (ASan + UBSan)
make clean    # supprime les .o
make fclean   # supprime les .o et l'exécutable
make re       # fclean puis all
```

Compilé avec `cc -Wall -Wextra -Werror`, testé sur Debian 12.

## Utilisation

```sh
./minishell
minishell$ ls -l | grep .c | wc -l
minishell$ cat < Makefile > copie.txt
minishell$ echo fin >> copie.txt
minishell$ cd /tmp
minishell$ exit 3
```

Il marche aussi sans terminal, en lisant les commandes sur l'entrée standard
(dans ce cas le prompt n'est pas affiché) :

```sh
printf 'echo bonjour | tr a-z A-Z\n' | ./minishell
```

## Ce qui est géré

- exécution des commandes avec recherche dans le `PATH` (`execvp`)
- builtins `cd` (sans argument : va dans `$HOME`) et `exit [code]`
- redirections `<`, `>` et `>>`, appliquées dans l'ordre comme bash
  (`echo a > x > y` crée `x` vide et écrit dans `y`)
- pipes avec autant de commandes que voulu : `cmd1 | cmd2 | cmd3 ...`
- les opérateurs peuvent être collés aux mots : `echo abc|rev`, `ls>out`
- code de retour comme bash : celui de la dernière commande du pipe,
  127 si la commande n'existe pas, 126 si elle n'est pas exécutable,
  128 + n si elle est tuée par le signal n, 2 pour une erreur de syntaxe
- erreurs de syntaxe détectées avant l'exécution (`| ls`, `ls |`,
  `echo a >`, `ls | | wc`)
- `Ctrl+C` affiche un nouveau prompt sans quitter le shell, et interrompt
  la commande en cours ; `Ctrl+\` est ignoré ; `Ctrl+D` quitte

Un builtin seul (`cd`, `exit`) tourne dans le processus du shell, sinon `cd`
ne servirait à rien. Dans un pipe il tourne dans un fils, comme dans bash :
`exit | cat` ne ferme pas le shell.

## Limites

- pas de guillemets : `echo "a b"` affiche `"a b"` avec les guillemets
- pas de variables d'environnement (`$HOME`, `$?`...)
- pas de `<<` (heredoc), de `&&`, `||`, `;` ni de `&`
- pas d'autres builtins (`echo`, `pwd`, `env`... passent par les
  programmes du système, ce qui suffit pour la plupart des usages)

## Organisation du code

| Fichier      | Rôle                                                      |
|--------------|-----------------------------------------------------------|
| `main.c`     | boucle principale : prompt, lecture avec `getline`        |
| `lexer.c`    | découpe la ligne en mots et opérateurs                    |
| `parse.c`    | construit le pipeline (commandes + redirections)          |
| `exec.c`     | crée les pipes, lance les fils, attend les codes retour   |
| `redir.c`    | ouvre les fichiers et fait les `dup2`                     |
| `builtins.c` | `cd` et `exit`                                            |
| `signals.c`  | gestion de `Ctrl+C` et `Ctrl+\`                           |
| `utils.c`    | petits utilitaires sur les tableaux de chaînes            |

Le déroulé pour une ligne : `tokenize` donne une liste de tokens, `parse_line`
les regroupe en un tableau de `t_cmd` (une par morceau entre deux `|`), puis
`exec_pipeline` lance un fils par commande. Chaque fils branche son entrée et
sa sortie sur les pipes, applique ses redirections, puis fait `execvp`.

## Tests

Les tests ont été faits à la main avec `make debug`, en comparant avec bash :
aucune erreur ni fuite signalée par les sanitizers, et pas de descripteur de
fichier qui reste ouvert (vérifié avec `ls /proc/self/fd`).
