#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "log.h"

void log_info(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

void log_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    printf("\n");
    va_end(args);
}

void log_fatal(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    printf("\n");
    va_end(args);

    exit(EXIT_FAILURE);
}
