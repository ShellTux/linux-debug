#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_ARGS 64
#define MAX_LINE 1024
#define MAX_PIPELINE 10

#define GREEN(message) "\033[32m" message "\033[0m"
#define RED(message) "\033[31m" message "\033[0m"

// Function to split the input line into commands separated by '|'
size_t split_by_pipe(char *line, char *commands[], const size_t max_commands) {
  size_t count = 0;

  for (char *token = strtok(line, "|"); token != NULL && count < max_commands;
       token = strtok(NULL, "|")) {
    commands[count++] = token;
  }

  return count;
}

int main() {
  // char line[MAX_LINE + 1];
  int exit_code = 0;

  for (char line[MAX_LINE + 1] = {0};;) {
    char prompt[1024] = {0};
    switch (exit_code) {
    case 0:
      snprintf(prompt, sizeof(prompt), "shell " GREEN("> "));
      break;
    default:
      snprintf(prompt, sizeof(prompt), "shell " RED("%d > "), exit_code);
      exit_code = 0;
      break;
    }

    fputs(prompt, stdout);
    fflush(stdout);

    // Read a line from user
    if (fgets(line, MAX_LINE, stdin) == NULL) {
      // End of file (Ctrl+D)
      break;
    }

    // Remove trailing newline
    line[strcspn(line, "\n")] = '\0';

    // Skip empty lines
    if (strlen(line) == 0) {
      continue;
    }

    // Make a copy of line because strtok modifies it
    char line_copy[MAX_LINE + 1] = {0};
    strncpy(line_copy, line, MAX_LINE + 1);

    // Split the line into commands separated by '|'
    char *commands[MAX_PIPELINE] = {0};
    const size_t num_commands =
        split_by_pipe(line_copy, commands, MAX_PIPELINE);

    // Parse each command into args
    int pipefds[2 * (num_commands - 1)];

    for (size_t i = 0; i < num_commands; i++) {
      // Parse command into args
      char *argv[MAX_ARGS + 1] = {0};
      int argc = 0;

      // Tokenize the command string
      char *cmd = commands[i];
      for (char *token = strtok(cmd, " "); token != NULL && argc < MAX_ARGS;
           token = strtok(NULL, " ")) {
        argv[argc++] = token;
      }
      argv[argc] = NULL;

      if (argv[0] == NULL)
        continue; // empty command

      // Handle built-in 'exit'
      if (strcmp(argv[0], "exit") == 0) {
        exit(0);
      }

      if (i == 0 && num_commands > 1 && pipe(pipefds + i * 2) == -1) {
        // first command in pipeline, create pipe
        perror("pipe");
        break;
      } else if (i > 0 && i < num_commands - 1 && pipe(pipefds + i * 2) == -1) {
        // middle command, create new pipe
        perror("pipe");
        break;
      }

      pid_t pid = fork();
      if (pid < 0) {
        perror("fork failed");
        break;
      } else if (pid == 0) {
        // Child process
        // If not first command, connect stdin to previous pipe read end
        if (i > 0 && dup2(pipefds[(i - 1) * 2], STDIN_FILENO) == -1) {
          perror("dup2 stdin");
          exit(EXIT_FAILURE);
        }

        // If not last command, connect stdout to current pipe write end
        if (i < num_commands - 1 &&
            dup2(pipefds[i * 2 + 1], STDOUT_FILENO) == -1) {
          perror("dup2 stdout");
          exit(EXIT_FAILURE);
        }

        // Close all pipe fds in child
        for (size_t j = 0; j < 2 * (num_commands - 1); j++) {
          close(pipefds[j]);
        }

        // Execute command
        execvp(argv[0], argv);
        perror("execvp");
        exit(EXIT_FAILURE);
      } else {
        // Parent process: close pipe ends that are no longer needed
        if (i > 0) {
          close(pipefds[(i - 1) * 2]);
        }
        if (i < num_commands - 1) {
          close(pipefds[i * 2 + 1]);
        }
      }
    }

    // Wait for all child processes
    for (size_t i = 0; i < num_commands; i++) {
      int status;
      wait(&status);

      if (WIFEXITED(status)) {
        exit_code = WEXITSTATUS(status);
      }
    }
  }

  printf("Exiting shell.\n");
  return 0;
}
