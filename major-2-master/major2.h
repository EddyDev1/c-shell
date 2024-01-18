#ifndef MAJOR2_H
#define MAJOR2_H

void cd(const char *path);
void showPath(char path[100][512], int num_paths);
void addPath(char path[100][512], const char *path_name, int num_paths);
void removePath(char path[100][512], const char *path_name, int num_paths);

#endif
