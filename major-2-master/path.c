#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "major2.h"

// when path is specified (no args)
void showPath(char path[100][512], int num_paths) {
  if (num_paths == 0) { // no paths
    printf("NO PATHS.\n");
  } else {
    printf("%s\n", path[0]);
  }
}

// when path + XXX is specified
void addPath(char path[100][512], const char *path_name, int num_paths) {
  if (num_paths < 100 && strlen(path[0]) < 512) { // number of paths and length of current path is checked 
    if (num_paths == 0)
      strncpy(path[0], path_name, strlen(path_name) - 1); // if it is the first path just copy it over
    else
      strcat(path[0], path_name); // other concat onto existing path

    path[0][strlen(path[0])] = '\0'; // add null terminator
    num_paths++; // increment num_paths
    showPath(path, num_paths); // display
  } else { // either number of paths was too large or the current path buffer limit was reached
    printf("LIMIT REACHED. REMOVE FIRST.\n"); 
  }
}

// when path - XXX is specified
void removePath(char path[100][512], const char *path_name, int num_paths) {
  if (num_paths != 0) { // need paths to remove
    int i, n = strlen(path[0]);

    if (strstr(path[0], path_name) == NULL) { // make sure path exists
      printf("PATH NOT FOUND.\n");
    } else {
      i = (int) (strstr(path[0], path_name) - path[0]);

      for (; i < n; i++)
        path[0][i] = '\0'; // fill that path with null terminators

      num_paths--; // decrease num_paths
      showPath(path, num_paths); // display
    }

  } else { // no paths present
    printf("NO PATHS TO REMOVE.\n");
  }
}
