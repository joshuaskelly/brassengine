# Tools
CC=gcc
AR='ar rcs'
RANLIB=ranlib

# Executable
BIN=brass

# Flags
CFLAGS=-Wall -std=c99 -O3

# Directories
SRC_DIR=src
BUILD_DIR=build
OBJ_DIR=$(BUILD_DIR)/obj
BIN_DIR=$(BUILD_DIR)/bin
WEB_DIR=$(BUILD_DIR)/web
PLATFORM_DIR=$(SRC_DIR)/platforms

# List of valid platforms
PLATFORMS=\
desktop-sdl2-opengl \
desktop-sdl2 \
desktop-sdl3 \
web \
web-opengl \

# Get targeted platform
PLATFORM=$(filter $(PLATFORMS), $(MAKECMDGOALS))

# Source files
BIN:=$(BIN_DIR)/$(BIN)
SRCS=$(wildcard $(SRC_DIR)/*.c) \
$(wildcard $(SRC_DIR)/collections/*.c) \
$(wildcard $(SRC_DIR)/graphics/*.c) \
$(wildcard $(SRC_DIR)/modules/*.c) \
$(wildcard $(SRC_DIR)/renderers/*.c) \
$(if $(PLATFORM), $(wildcard $(PLATFORM_DIR)/$(PLATFORM)/*.c),)

OBJS= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Libraries
INC=-Ilibs

LUA_DIR=libs/lua
LIBLUA=$(LUA_DIR)/liblua.a

GIFLIB_DIR=libs/giflib
LIBGIF=$(GIFLIB_DIR)/libgif.a

ZIP_DIR=libs/zip
LIBZIP=$(ZIP_DIR)/zip.a

CJSON_DIR=libs/cjson
LIBCJSON=$(CJSON_DIR)/libcjson.a

MATHC_DIR=libs/mathc
LIBMATHC=$(MATHC_DIR)/libmathc.a

LIBS=$(LIBLUA) $(LIBGIF) $(LIBZIP) $(LIBCJSON) $(LIBMATHC)
LDLIBS=$(LIBS) -lm

# Debug specific makefile
ifdef DEBUG
include debug.mk
endif

# Platform specific makefile. Optional.
ifneq ($(PLATFORM),)
-include $(PLATFORM_DIR)/$(PLATFORM)/platform.mk
endif

# Targets
default:help

all:$(BIN)

desktop-sdl2:all ## Build desktop platform

desktop-sdl2-opengl:all ## Build desktop OpenGL ES 2.0 platform

desktop-run: ## Run desktop build
	./$(BIN)

desktop-sdl3:all ## Build sdl3 desktop platform

web: ## Build web platform

web-opengl: ## Build web OpenGL ES 2.0 platform

web-run: ## Run web build
	emrun $(WEB_DIR)/index.html

$(BIN): $(OBJS) | $(BIN_DIR) $(LIBS)
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
	cd $(CJSON_DIR) && make libcjson.a CC=$(CC) AR=$(AR) RANLIB=$(RANLIB)

$(LIBMATHC):
	cd $(MATHC_DIR) && make CC=$(CC) AR=$(AR) RANLIB=$(RANLIB)

docs: ## Generate API documentation
	ldoc src/modules -d docs -p brassengine -f markdown

docsclean: ## Delete generated API documentation
	rm -rf docs

defs: ## Generate language server definition files
	ldoc src/modules --filter ./scripts/library.filter

defsclean: ## Delete generated language server definition files
	rm -rf library
	mkdir library

mostlyclean: ## Deletes project auto generated files
	find ./build/ -maxdepth 8 -type f -delete
clean: ## Deletes all auto generated files
	rm -rf $(BUILD_DIR)
	mkdir $(BUILD_DIR)
	mkdir $(BUILD_DIR)/bin
	mkdir $(OBJ_DIR)
	mkdir $(OBJ_DIR)/collections
	mkdir $(OBJ_DIR)/graphics
	mkdir $(OBJ_DIR)/modules
	mkdir $(OBJ_DIR)/platforms
	mkdir $(OBJ_DIR)/platforms/desktop-sdl2-opengl
	mkdir $(OBJ_DIR)/platforms/desktop-sdl2
	mkdir $(OBJ_DIR)/platforms/desktop-sdl3
	mkdir $(OBJ_DIR)/platforms/web
	mkdir $(OBJ_DIR)/platforms/web-opengl
	mkdir $(OBJ_DIR)/renderers
	mkdir $(BUILD_DIR)/web
	cd $(LUA_DIR) && make clean
	cd $(GIFLIB_DIR) && make clean
	cd $(ZIP_DIR) && make clean
	cd $(CJSON_DIR) && make clean
	cd $(MATHC_DIR) && make clean

help: ## Show help prompt
	@echo "usage:"
	@echo "  make \033[32m<target>\033[0m"
	@echo ""
	@echo "targets:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[32m%-30s\033[0m %s\n", $$1, $$2}'
