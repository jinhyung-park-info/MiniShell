#ifndef __EXECUTE_H__
#define __EXECUTE_H__

#include "miniShell.h"

// Implements the cd command
int change_directory(char** words);

// Executes a non-built-in command (commands that are not cd, exit)
int execute_nonbuilt_in_command(command *s);

// Executes a command
int execute_command(command *cmd);

#endif
