#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "major2.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <pwd.h>

void cd(const char *path) { // If no path is provided, use the HOME directory
  if (path == NULL) {
    // using the  getpwuid to get the home directory of the user
    struct passwd *pwd = getpwuid(getuid());
    if (pwd != NULL && pwd->pw_dir != NULL) {
      path = pwd->pw_dir;
    } else {
      // If getpwuid fails or home directory is NULL, try using the HOME
      // environment variable
      const char *home = getenv("HOME");

      if (home == NULL) {
        fprintf(stderr, "Error: Could not determine home directory.\n");
        return;
      }
      path = home;
    }
  }
  if (strcmp(path, "..") == 0) {
    // changing to parent directory
    if (chdir("..") != 0) {
      perror("Error changing to parent directory");
      exit(EXIT_FAILURE);
    }
  }
}
