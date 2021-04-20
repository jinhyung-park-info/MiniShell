#ifndef __PARSER_H__
#define __PARSER_H__

#include "miniShell.h"

// Prints a prompt accordint to the assignment format
int print_prompt();

// Preprocess the command line - delete newline, verify if background command
int preprocess_command_line(char* command_line);

// Tokenize command into tokens
void tokenize_command_line(char *command_line, char **tokens);

// Verify if the given command is a built-in command (cd, exit)
int is_built_in_command(char *token);

// Verify if the given path is an absolute path
int is_absolute_path(char* path);

// Verify if the given command contains stdin/stdout redirections.
// If it does, delete it from the token and store it in the command struct
int process_redirection_signals(char** tokens, command* cmd);

// Constructs a command struct for execution
command* build_command(char** tokens, int is_background);

// Free heap area that was allocated to command struct
void free_command(command *cmd);

#endif
