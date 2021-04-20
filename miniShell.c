#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <mcheck.h>
#include <unistd.h>

#include "parser.h"
#include "miniShell.h"
#include "execute.h"

int main(int argc, char** argv) {

        char command_line[MAX_CMD];
        char *tokens[MAX_TOKEN];
        int is_background;

        while (1) {

                // 1. Prints Prompt
                if (print_prompt() == -1) { break; }

                // 2. Reads input (raw command)
                fgets(command_line, MAX_CMD, stdin);

                // 3. Preprocess - eliminate new line, ampersand
                is_background = preprocess_command_line(command_line);

                // 4. Parse the command into tokens
                tokenize_command_line(command_line, tokens);
                if (!(*tokens)) {
                        continue;
                }

                // 5. Build the command in a formatted way
                command *scmd = build_command(tokens, is_background);

                // 6. Execute the command
                int exitcode = 0;
                exitcode = execute_command(scmd);
                if (exitcode == -1) {
                        break;
                }

                // 7. Free heap area allocated for command string
                free_command(scmd);
        }

        return 0;
}
