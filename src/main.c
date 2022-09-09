#include "arguments.h"
#include "platform.h"

int main(int argc, char* argv[]) {
    arguments_set(argc, argv);

    return platform_main(argc, argv);
}
