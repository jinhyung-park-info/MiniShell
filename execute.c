#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "parser.h"
#include "execute.h"
#include "miniShell.h"



int execute_command(command *cmd) {

    // Executes a command

    int status, exitcode;

    // Case #1. If the given command is a built-in command
    if (cmd->builtin != 0) {

        // Case #1-1 - cd command
        if (cmd->builtin == CHANGE_DIR_CMD) {
            exitcode = change_directory(cmd->tokens);
        }

        // Case #1-2 - exit command
        else if (cmd->builtin == EXIT_CMD){
            exit(0);
        }
    }

    // Case #2. If the given command is a non-built-in command
    else {
            pid_t pid;

            // Case #2-1: Fork Failed
            if ((pid = fork()) == -1) {
            	perror("fork() failed\n");
            	return 1;

			// Case #2-2: Child process
            } else if (pid == 0) {
            	exit(execute_nonbuilt_in_command(cmd));

            // Case #2-3: Parent process
            } else {
            	if (!cmd->is_background) {
            		waitpid(pid, &status, 0);
            	} else {
            		cmd->is_background = 0;
            	}
            }

    }

    return exitcode;
}


int change_directory(char** cmd) {

    // Implements the cd command

    char *new_dir;

    // Error check
    if (cmd == NULL || cmd[0] == NULL || strcmp(cmd[0], "cd") != 0 || cmd[1] == NULL) {
        return 1;
    }

    if (is_absolute_path(cmd[1])) {
    	// Case #1 : Absolute Path -> change directory directly
        if (chdir(cmd[1]) == -1){
            perror("chdir() failed\n");
            return 1;
        }

    } else {
		// Case #2 : Relative Path -> add to the current directory
        if ((new_dir = malloc(MAX_DIRNAME)) != NULL) {
            getcwd(new_dir, MAX_DIRNAME);
            strcat(new_dir, "/");
          	strcat(new_dir, cmd[1]);

            if (chdir(new_dir) == -1) {
                perror("chdir() failed\n");
                return 1;
        	}
        } else {
        	perror("malloc() failed\n");
        	return 1;
        }

    }

    return 0;
}


int execute_nonbuilt_in_command(command *s) {
    
    // Executes a non-built-in command (commands that are not cd, exit)

    // 1. If the command contains stdin, stdout redirections, process it first
    if (s->stdin_redirect != NULL && s->stdout_redirect != NULL) {

        // Case #1 : If the command contains both stdin and stdout redirection

        int fd1, fd2;

        if ((fd1 = open(s->stdin_redirect, O_RDONLY, 0777)) == -1){
            perror("open failed()\n");
            return 1;
        }

        // Call dup2() for stdin redirection
        if (dup2(fd1, STDIN_FILENO) == -1){
            perror("dup2() failed\n");
            return 1;
        }

        if (close(fd1) == -1){
            perror ("close() failed\n");
            return 1;
        }

        if ((fd2 = open(s->stdout_redirect, O_WRONLY|O_TRUNC|O_CREAT, 0777)) == -1){
            perror("open() failed\n");
            return 1;
        }

        // Call dup2() for stdin redirection
        if (dup2(fd2, STDOUT_FILENO) == -1){
            perror ("dup2() failed\n");
            return 1;
        }

        if (close(fd2) == -1){
            perror ("close() failed\n");
            return 1;
        }

    } else if (s->stdin_redirect != NULL){

        // Case #2 : If the command only contains stdin redirection

        int fd1;

        if ((fd1 = open(s->stdin_redirect, O_RDONLY, 0777)) == -1){
            perror("open() failed\n");
            return 1;
        }

        // Call dup2() for stdin redirection
        if(dup2(fd1, STDIN_FILENO) == -1){
            perror ("dup2() failed\n");
            return 1;
        }

        if (close(fd1) == -1){
            perror ("close() failed\n");
            return 1;
        }


    } else if (s->stdout_redirect != NULL) {

        // Case #3 : If the command only contains stdout redirection

        int fd2;


        if ((fd2 = open(s->stdout_redirect, O_WRONLY|O_TRUNC|O_CREAT, 0777)) == -1){
            perror("open() failed\n");
            return 1;
        }

        // Call dup2() for stdin redirection
        if (dup2(fd2, STDOUT_FILENO) == -1){
            perror("dup2() failed\n");
            return 1;
        }

        if (close(fd2) == -1){
            perror("close() failed\n");
            return 1;
        }


    } else if (s->stderr_redirect != NULL) {

        // Case #4 : If the command only contains stderr redirection

        int fd3;


        if ((fd3 = open(s->stderr_redirect, O_WRONLY|O_TRUNC|O_CREAT, 0777)) == -1){
            perror("open() failed\n");
            return 1;
        }

        // Call dup2() for stdin redirection
        if (dup2(fd3, STDERR_FILENO) == -1){
            perror("dup2() failed\n");
            return 1;
        }

        if (close(fd3) == -1){
            perror("close() failed\n");
            return 1;
        }
    }

    // Execute the command by passing it to execvp command
	if (execvp(s->tokens[0], s->tokens) == -1){
	    perror("exevp() failed\n");
	    return 1;
	}

    return 0;


}
