#ifndef _MINISHELL_H
#define _MINISHELL_H

/* built-in commands */
#define MAX_PROMPT   1024
#define CHANGE_DIR_CMD   1
#define EXIT_CMD 2
#define MAX_CMD 1024
#define MAX_DIRNAME 1024
#define MAX_TOKEN 128

typedef struct command_t {
        char *stdin_redirect;    // Stdin Redirection
        char *stdout_redirect;   // Stdout Redirection
        char *stderr_redirect;   // Stderr Redirection
        char **tokens;           // tokens
        int builtin;             // Is the command built-in command (cd, exit)
        int is_background;       // Is the command should be executed in the background
} command;

#endif
