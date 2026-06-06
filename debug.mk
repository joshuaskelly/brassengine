# Debug specific variables and targets

# Enable console output for Windows
ifeq ($(OS),Windows_NT)
LDLIBS+=-mconsole
endif

CFLAGS=-Wall -std=c99 -DDEBUG -g
