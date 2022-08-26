#include <stdio.h>
#include <stdlib.h>

#include "log.h"

void log_info(char* message) {
    printf("%s", message);
}


void log_error(char* message) {
    fprintf(stderr, "\x1B[31m%s\033[0m", message);
}

void log_fatal(char* message) {
    fprintf(stderr, "\x1B[31m%s\033[0m", message);
    exit(EXIT_FAILURE);
}
