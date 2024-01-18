#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "major2.h"

#define MAX_INPUT 512
#define NUM_BUILTINS 4

char path[100][MAX_INPUT]; // path
int num_paths = 0;         // amount of paths
char *builtins[] = {"myhistory", "alias", "path", "cd"};

void exec_com(char *input);
void exec_builtin(char *input);
void handle_pipe(char *input);

int main(int argc, char *argv[]) {
  char user_input[MAX_INPUT];
  char *copy_input, *getpathinit;
  int pathinit = 0;

  // initializing path
  realpath(argv[0], path[0]);
  getpathinit = strrchr(path[0], '/');
  pathinit = (int)(getpathinit - path[0]);
  path[0][pathinit] = ':';
  for (int i = pathinit; i < strlen(path[0]); i++)
    path[0][i] = '\0';

  if (argc == 2) { // batch mode

    FILE *f = fopen(argv[1], "r"); // open file entered

    if (f == NULL) {
      fprintf(stderr, "%s", "Not able to open the file.\n");
      exit(1);
    }

    while (fgets(user_input, MAX_INPUT, f) != NULL) { // read from file
      if (!(strlen(user_input) < MAX_INPUT)) {
        fprintf(stderr, "%s",
                "Command line is over 512 characters long, exceeding buffer "
                "limit.");
      }

      if (strstr(user_input, "|") == NULL) { // no pipe
        handle_pipe(user_input);
      }

      else { // pipe and tokenize input for exec
        char *pipe = strtok(user_input, ";");
        while (pipe != NULL) {
          handle_pipe(pipe);
          pipe = strtok(NULL, ";");
        }
      }
    }

    return 0;
  }

  while (1) { // interactive

    printf("major2> ");
    fgets(user_input, sizeof(user_input), stdin); // read input
    user_input[strcspn(user_input, "\n")] = '\0';

    if (!(strlen(user_input) < MAX_INPUT)) {
      fprintf(stderr, "%s",
              "Command line is over 512 characters long, exceeding buffer "
              "limit.");
      // notify user if input is too long
    }

    // builtins that dont need to be piped
    if (strncmp(user_input, "cd", 2) == 0 || strncmp(user_input, "path", 4) == 0){
      exec_builtin(user_input);
      continue; // avoid falling through
    } 

    if (strstr(user_input, "|") == NULL) {
      handle_pipe(user_input);
    } else {
      char *pipe = strtok(user_input, ";");
      while (pipe != NULL) { // tokenize
        handle_pipe(pipe);
        pipe = strtok(NULL, ";");
      }
    }
  }

  return 0;
}

// for redirection and execvp
void exec_com(char *input) {
  char *command[MAX_INPUT];
  char *cmdTok = NULL;

  if (strstr(input, " ")) {
    cmdTok = strtok(input, " ");
  }
  int i = 0;
  if (cmdTok == NULL) {
    command[i] = input;
    i++;
  }
  while (cmdTok != NULL) { // tokenize
    command[i] = cmdTok;
    i++;
    cmdTok = strtok(NULL, " ");
  }

  command[i] = NULL;

  int redir = 0; // 1 FOR OUTPUT 2 FOR INPUT
  char *redir_target;
  for (int h = 0; h < i; h++) { // search for redirection
    if (strcmp(command[h], ">") == 0) {
      redir = 1;
      redir_target = command[h + 1];
      command[h] = NULL;
    }

    else if (strcmp(command[h], "<") == 0) {
      redir = 2;
      redir_target = command[h + 1];
      command[h] = NULL;
    }
  }

  pid_t pid = fork();

  if (pid == 0) {
    // Child

    if (redir == 1) {
      // OUTPUT REDIRECT

      int fd = open(redir_target, O_WRONLY | O_CREAT | O_TRUNC, 0666);
      dup2(fd, STDOUT_FILENO);
      close(fd);
    }

    else if (redir == 2) {
      // INPUT REDIRECT
      int fd = open(redir_target, O_WRONLY | O_CREAT | O_TRUNC, 0666);
      dup2(fd, STDIN_FILENO);
      close(fd);
    }

    // command had no pipe/redirect, just execute it
    execvp(command[0], command);
    perror("execvp");
    exit(EXIT_FAILURE);
  }

  else if (pid > 0) {
    // Parent
    wait(&pid);
  }

  else {
    // Error
    perror("fork");
    exit(EXIT_FAILURE);
  }
}

// for pipe
void handle_pipe(char *input) {
  char *tokenizedCommand[MAX_INPUT];
  char *pTok = NULL;

  if (strstr(input, "|") != NULL) {
    pTok = strtok(input, "|");
  }
  int i = 0;

  if (pTok == NULL) {
    tokenizedCommand[i] = input;
    i++;
  }
  while (pTok != NULL) {
    tokenizedCommand[i] = pTok;
    i++;
    pTok = strtok(NULL, "|");
  }

  tokenizedCommand[i] = NULL;

  int pipes[i - 1][2]; // creates i-1 pipes

  for (int j = 0; j < i; j++) {
    if (j < (i - 1)) {
      pipe(pipes[j]);
    }

    pid_t pid = fork();

    if (pid == 0) {
      // Child

      if (j > 0) {
        dup2(pipes[j - 1][0], STDIN_FILENO);
        close(pipes[j - 1][0]);
        close(pipes[j - 1][1]);
      }

      if (j < (i - 1)) {
        dup2(pipes[j][1], STDOUT_FILENO);
        close(pipes[j][0]);
        close(pipes[j][1]);
      }

      if (strcmp(tokenizedCommand[j], "myhistory") == 0 ||
          strcmp(tokenizedCommand[j], "alias") == 0) {
        exec_builtin(tokenizedCommand[j]);
        // builtins that need piping before execution
      }

      else {
        // no pipe, search for redirect next
        exec_com(tokenizedCommand[j]);
      }
      exit(0);
    }

    else if (pid > 0) {
      // Parent
      if (j > 0) {
        close(pipes[j - 1][0]);
        close(pipes[j - 1][1]);
      }

      wait(&pid);

    }

    else {
      perror("fork");
      exit(1);
    }
  }
}

// for builtins
void exec_builtin(char *input) {
  char *cmd[MAX_INPUT];
  char *cmdTok = NULL;

  int i = 0;

  for (int i = 0; i < NUM_BUILTINS; i++) {
    if (strstr(input, builtins[i])) {
      break;
    }
  }

  if (strstr(input, " ")) {
    int j = 0;

    cmdTok = strtok(input, " ");
    while (cmdTok != NULL) { // basic space tokenizer
      cmd[j] = cmdTok;
      j++;
      cmdTok = strtok(NULL, " ");
    }
  }

  switch (i) // 0 - myhistory, 1 - alias, 2 - path, 3- cd
  {
  case 0: {
      

  } break;
  case 1: {

  } break;
  case 2: {
    // cmd[0] = "path", [1] = "+ OR - OR NULL", [2] = "args or NULL"
    if (strcmp(cmd[1], "+") == 0) {
      if (cmd[2][0] == '.')
        cmd[2][0] = ':';

      addPath(path, cmd[2], num_paths);
      showPath(path, num_paths);
    } else if (strcmp(cmd[1], "-")) {
      if (cmd[2][0] == '.') // formatting
        cmd[2][0] = ':';

      removePath(path, cmd[2], num_paths);
    } else if (cmd[1] == NULL) {
      showPath(path, num_paths);
    }
  } break;
  case 3: {
    // cmd[0] will be "cd"
    // cmd[1] will either be a new directory, ".." for parent directory or NULL
    // for home directorty
    if (strcmp(cmd[0], "cd") == 0) {
      char *input = cmd[1];
      if (input != NULL)
        cd(input);
      else
        cd(NULL);
    }
  } break;
  default:
    break;
  }
}