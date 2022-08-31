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
SRCS=$(wildcard $(SRC_DIR)/*.c) $(if $(PLATFORM), $(PLATFORM_DIR)/$(PLATFORM).c,)

OBJS= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
INC=-Ilibs

LUA_DIR=libs/lua
LIBLUA=$(LUA_DIR)/liblua.a

CFLAGS=-Wall -std=c99 -O3
DFLAGS=-Wall -std=c99 -DDEBUG -g
LFLAGS=
LDLIBS=$(LIBLUA) -lSDL2

none:
	@echo "Please do 'make PLATFORM' where PLATFORM is one of these:"
	@echo "   $(PLATFORMS)"

all:$(BIN)

debug:CFLAGS=$(DFLAGS)
debug:all

desktop:all

web:CC=emcc -s USE_SDL=2
web:LFLAGS=AR='emar rcu' RANLIB=emranlib
web: $(OBJS) | $(BIN_DIR) $(LIBLUA)
	@echo "SRCS = $(SRCS)"
	$(CC) $^ $(LIBLUA) -o $(WEB_DIR)/main.html --embed-file assets

$(BIN): $(OBJS) | $(BIN_DIR) $(LIBLUA)
	$(CC) $(CFLAGS) $(INC) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(LIBLUA):
	cd $(LUA_DIR) && make a CC=$(CC) $(LFLAGS)

run:
	./$(BIN)

serve:
	emrun $(WEB_DIR)/main.html

mostlyclean:
	find ./build/ -maxdepth 3 -type f -delete

clean:
	find ./build/ -maxdepth 3 -type f -delete
	cd $(LUA_DIR) && make clean
