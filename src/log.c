#include <stdio.h>
#include <stdlib.h>

#include "log.h"

void log_info(const char* message) {
    printf("%s\n", message);
}


void log_error(const char* message) {
    fprintf(stderr, "\x1B[31m%s\033[0m\n", message);
}

void log_fatal(const char* message) {
    fprintf(stderr, "\x1B[31m%s\033[0m\n", message);
    exit(EXIT_FAILURE);
}
