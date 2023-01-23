#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "console.h"
#include "log.h"

void log_info(const char* format, ...) {
    // TODO: Reduce code duplication.
    va_list args1;
    va_list args2;

    va_start(args1, format);
    va_copy(args2, args1);
    const int count = 1 + vsnprintf(NULL, 0, format, args1);
    char buffer[count];
    va_end(args1);
    vsnprintf(buffer, sizeof(buffer), format, args2);
    va_end(args2);

    fprintf(stdout, "%s\n", buffer);
    console_buffer_write(buffer);
}

void log_error(const char* format, ...) {
    va_list args1;
    va_list args2;

    va_start(args1, format);
    va_copy(args2, args1);
    const int count = 1 + vsnprintf(NULL, 0, format, args1);
    char buffer[count];
    va_end(args1);
    vsnprintf(buffer, sizeof(buffer), format, args2);
    va_end(args2);

    fprintf(stderr, "%s\n", buffer);
    console_buffer_write(buffer);
}

void log_fatal(const char* format, ...) {
    va_list args1;
    va_list args2;

    va_start(args1, format);
    va_copy(args2, args1);
    const int count = 1 + vsnprintf(NULL, 0, format, args1);
    char buffer[count];
    va_end(args1);
    vsnprintf(buffer, sizeof(buffer), format, args2);
    va_end(args2);

    fprintf(stderr, "%s\n", buffer);
    console_buffer_write(buffer);

    exit(EXIT_FAILURE);
}
