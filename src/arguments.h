#ifndef ARGUMENTS_H
#define ARGUMENTS_H

void arguments_set(int argc, char* argv[]);

int arguments_count(void);

char** arguments_vector(void);

int arguments_check(char* arg);

#endif
