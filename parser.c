#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "parser.h"
#include "miniShell.h"


int print_prompt() {

    // Prints a prompt according to the assignment format
    // Format = [HH:MM:SS]username@directory

    char current_time[MAX_PROMPT];
    char username[MAX_PROMPT];
    char current_directory[MAX_PROMPT];

    time_t raw_time;
    struct tm *time_struct;

    // 1. Get Current Time in HH::MM::SS format
    time(&raw_time);
    time_struct = localtime(&raw_time);
    strftime(current_time, sizeof(current_time), "%X", time_struct);

    // 2. Get Current Username
    if (getlogin_r(username, sizeof(username)) == -1) {
        perror("getlogin_r() error");
        return -1;
    }

    // 3. Get Current Directory
    if (getcwd(current_directory, sizeof(current_directory)) == NULL) {
        perror("getcwd() error");
        return -1;
    }

    printf("[%s]%s@~%s$", current_time, username, current_directory);

    return 0;
}

int preprocess_command_line(char* command_line) {

    // Preprocess the command line - delete newline, verify if background command

    int is_background;

    // 1. Deletes newline character
    if (command_line[strlen(command_line) - 1] == '\n') {
        command_line[strlen(command_line) - 1] = '\0';
    }

    // 2. Verify if background command
    if (command_line[strlen(command_line) - 1] == '&') {
        command_line[strlen(command_line) - 1] = '\0';
        is_background = 1;

    } else {
        is_background = 0;
    }

    return is_background;
}



void tokenize_command_line(char *command_line, char **tokens) {

    // Tokenize command into tokens

    while (*command_line != '\0') {

        // 1. Break a single command string into tokens by deleting spaces between words to null character
        while (*command_line == '\t' || *command_line == '\n' || *command_line == ' ') {
            *command_line++ = '\0';
        }

        if (*command_line == '\0') { break; }

        // 2. Store the relative location of a word(token) in the command
        *tokens++ = command_line;

        while (*command_line != '\0' && *command_line != '\t' && *command_line != '\n' && *command_line != ' ') {
                command_line++;
        }
    }

    *tokens = '\0';

}

command* build_command(char** tokens, int is_background) {

    // Constructs a command struct for execution

    // 1. Initialize the command struct
    command *cmd_ptr = malloc(sizeof(command));
    cmd_ptr->stdin_redirect = NULL;
    cmd_ptr->stdout_redirect = NULL;
    cmd_ptr->stderr_redirect = NULL;
    cmd_ptr->tokens = NULL;
    cmd_ptr->is_background = is_background;
    cmd_ptr->builtin = is_built_in_command(tokens[0]);

    // 2. Verify if the given command contains any redirections
    int err = process_redirection_signals(tokens, cmd_ptr);
    if (err == -1) {
        printf("Wrong command format\n");
        return NULL;
    }

    return cmd_ptr;
}


int is_built_in_command(char *token) {

    // Verify if the given command is a built-in command (cd, exit)

    // 1. Verify if the given command is change directory command
    if (strcmp(token, "cd") == 0) {
        return CHANGE_DIR_CMD;
    }

    // 2. Verify if the given command is exit command
    if (strcmp(token, "exit") == 0) {
        return EXIT_CMD;
    }

    return 0;
}


int is_absolute_path(char* path) {

    // Verify if the given path is an absolute path

    return (path[0] == '/');
}

int process_redirection_signals(char** tokens, command* cmd) {

    // Verify if the given command contains stdin/stdout redirections.
    // If it does, delete it from the token and store it in the command struct

    int deleted_indices = 0;
    int contains_redirection = 0;
    int idx = 0;

    // 1. Loop over the tokens to check if the command contains redirection signal
    while (tokens[idx]) {

        contains_redirection = 0;

        // Case #1. If stdout redirection signal is included
        if (!strcmp(tokens[idx], ">")) {
            if (!tokens[idx + 1]) {
                // If there is no token after stdout redirection signal, it is a wrong command
                printf("Please type an argument for stdout redirection\n");
                return -1;
            }

            cmd->stdout_redirect = tokens[idx + 1];
            deleted_indices += 2;
            contains_redirection = 1;
        }

        // Case #2. If stdout redirection signal is included
        if (!strcmp(tokens[idx], "<")) {
            if (!tokens[idx + 1]) {
                // If there is no token after stdin redirection signal, it is a wrong command
                printf("Please type an argument for stdin redirection\n");
                return -1;
            }

            cmd->stdin_redirect = tokens[idx + 1];
            deleted_indices += 2;
            contains_redirection = 1;
        }

        if (contains_redirection) {
            // Increase one more idx since the next argument won't be a redirection signal
            idx++;
        }

        idx++;
    }

    // 2. Copy the update the tokens back to command struct
    cmd->tokens = malloc((idx - deleted_indices + 1) * sizeof(char*));

    int dest_idx = 0;
    int src_idx = 0;
    while (tokens[src_idx]) {
        if (!strcmp(tokens[src_idx], "<") || !strcmp(tokens[src_idx], ">")) {
            src_idx += 2;
         } else {
            cmd->tokens[dest_idx++] = tokens[src_idx++];
         }
    }

    cmd->tokens[dest_idx] = NULL;

    return 0;
}



void free_command(command *cmd) {

    // Free heap area that was allocated to command struct

    if (cmd->tokens) {
        free(cmd->tokens);
    }

}
