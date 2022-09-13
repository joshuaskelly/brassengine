#include <string.h>

#include "arguments.h"

static int argc;
static char** argv;

void arguments_set(int argc_, char* argv_[]) {
    argc = argc_;
    argv = argv_;
}

int arguments_count(void) {
    return argc;
}

const char** arguments_vector(void) {
    return (const char**)argv;
}

const char* arguments_last(void) {
    return (const char*)argv[argc - 1];
}

int arguments_check(const char* arg) {
    for (int i = 0; i < argc; i++) {
        if (!strcmp(arg, argv[i])) {
            return i;
        }
    }

    return 0;
}
