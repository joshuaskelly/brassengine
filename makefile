CC=gcc
BIN=toy

SRC_DIR=src
BUILD_DIR=build
OBJ_DIR:=$(BUILD_DIR)/obj
BIN_DIR:=$(BUILD_DIR)/bin
WEB_DIR:=$(BUILD_DIR)/web
PLATFORM_DIR=$(SRC_DIR)/platforms

PLATFORMS=desktop web
PLATFORM=$(filter $(PLATFORMS), $(MAKECMDGOALS))

BIN:=$(BIN_DIR)/$(BIN)
SRCS=$(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/renderers/*.c) $(wildcard $(SRC_DIR)/bindings/*.c) $(if $(PLATFORM), $(PLATFORM_DIR)/$(PLATFORM).c,)

OBJS= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
INC=-Ilibs

LUA_DIR=libs/lua
LIBLUA=$(LUA_DIR)/liblua.a

GIFLIB_DIR=libs/giflib
LIBGIF=$(GIFLIB_DIR)/libgif.a

ZIP_DIR=libs/zip
LIBZIP=$(ZIP_DIR)/zip.a

CFLAGS=-Wall -std=c99 -O3
DFLAGS=-Wall -std=c99 -DDEBUG -g
LFLAGS=
LDLIBS=$(LIBLUA) $(LIBGIF) $(LIBZIP) -lSDL2 -lm

default:help

all:$(BIN)

desktop:all ## Build desktop platform

debug:CFLAGS=$(DFLAGS)
debug:all

desktop-run: ## Run desktop build
	./$(BIN)

web:CC=emcc -s USE_SDL=2 -s USE_GIFLIB=1
web:LFLAGS=AR='emar rcu' RANLIB=emranlib
web: $(OBJS) | $(BIN_DIR) $(LIBLUA) ## Build web platform
	@echo "SRCS = $(SRCS)"
	$(CC) $^ $(LIBLUA) $(LIBZIP) -o $(WEB_DIR)/main.html --embed-file assets

web-run: ## Run web build
	emrun $(WEB_DIR)/main.html

$(BIN): $(OBJS) | $(BIN_DIR) $(LIBLUA) $(LIBGIF)
	$(CC) $(CFLAGS) $(INC) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(LIBLUA):
	cd $(LUA_DIR) && make a CC=$(CC) $(LFLAGS)

$(LIBGIF):
	cd $(GIFLIB_DIR) && make libgif.a CC=$(CC) $(LFLAGS)

mostlyclean: ## Deletes project auto generated files
	find ./build/ -maxdepth 3 -type f -delete

clean: ## Deletes all auto generated files
	find ./build/ -maxdepth 3 -type f -delete
	cd $(LUA_DIR) && make clean
	cd $(GIFLIB_DIR) && make clean

help: ## Show help prompt
	@echo "usage:"
	@echo "  make \033[32m<target>\033[0m"
	@echo ""
	@echo "targets:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[32m%-30s\033[0m %s\n", $$1, $$2}'
