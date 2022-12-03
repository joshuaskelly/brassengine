CC=gcc
AR='ar rcs'
RANLIB=ranlib
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
SRCS=$(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/renderers/*.c) $(wildcard $(SRC_DIR)/modules/*.c) $(if $(PLATFORM), $(PLATFORM_DIR)/$(PLATFORM).c,)

OBJS= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
INC=-Ilibs

LUA_DIR=libs/lua
LIBLUA=$(LUA_DIR)/liblua.a

GIFLIB_DIR=libs/giflib
LIBGIF=$(GIFLIB_DIR)/libgif.a

ZIP_DIR=libs/zip
LIBZIP=$(ZIP_DIR)/zip.a

CJSON_DIR=libs/cjson
LIBCJSON=$(CJSON_DIR)/libcjson.a

CFLAGS=-Wall -std=c99 -O3
DFLAGS=-Wall -std=c99 -DDEBUG -g
LDLIBS=$(LIBLUA) $(LIBGIF) $(LIBZIP) $(LIBCJSON) -lSDL2 -lm -lssp

default:help

all:$(BIN)

desktop:all ## Build desktop platform

debug:CFLAGS=$(DFLAGS)
debug:all

desktop-run: ## Run desktop build
	./$(BIN)

web:CC=emcc -s USE_SDL=2 -s USE_GIFLIB=1
web:AR='emar rcu'
web:RANLIB=emranlib
web: $(OBJS) | $(BIN_DIR) $(LIBLUA) $(LIBZIP) $(LIBCJSON) ## Build web platform
	@echo "SRCS = $(SRCS)"
	$(CC) $^ $(LIBLUA) $(LIBZIP) -o $(WEB_DIR)/main.html --embed-file assets

web-run: ## Run web build
	emrun $(WEB_DIR)/main.html

$(BIN): $(OBJS) | $(BIN_DIR) $(LIBLUA) $(LIBGIF) $(LIBZIP) $(LIBCJSON)
	$(CC) $(CFLAGS) $(INC) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(LIBLUA):
	cd $(LUA_DIR) && make a CC=$(CC) AR=$(AR) RANLIB=$(RANLIB)

$(LIBGIF):
	cd $(GIFLIB_DIR) && make libgif.a CC=$(CC)

$(LIBZIP):
	cd $(ZIP_DIR) && make zip.a CC=$(CC) AR=$(AR) RANLIB=$(RANLIB)

$(LIBCJSON):
	cd $(CJSON_DIR) && make static CC=$(CC) RANLIB=$(RANLIB)

mostlyclean: ## Deletes project auto generated files
	find ./build/ -maxdepth 3 -type f -delete

clean: ## Deletes all auto generated files
	rm -rf $(BUILD_DIR)
	mkdir $(BUILD_DIR)
	mkdir $(BUILD_DIR)/bin
	mkdir $(BUILD_DIR)/obj
	mkdir $(BUILD_DIR)/obj/modules
	mkdir $(BUILD_DIR)/obj/platforms
	mkdir $(BUILD_DIR)/obj/renderers
	mkdir $(BUILD_DIR)/web
	cd $(LUA_DIR) && make clean
	cd $(GIFLIB_DIR) && make clean
	cd $(ZIP_DIR) && make clean
	cd $(CJSON_DIR) && make clean

help: ## Show help prompt
	@echo "usage:"
	@echo "  make \033[32m<target>\033[0m"
	@echo ""
	@echo "targets:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[32m%-30s\033[0m %s\n", $$1, $$2}'
