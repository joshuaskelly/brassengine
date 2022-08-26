CC:=gcc
BIN:=toy

SRC_DIR:=src
BUILD_DIR:=build
OBJ_DIR:=$(BUILD_DIR)/obj
BIN_DIR:=$(BUILD_DIR)/bin

LUA_DIR:=libs/lua

BIN:=$(BIN_DIR)/$(BIN)
SRCS:=$(wildcard $(SRC_DIR)/*.c)
OBJS:= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
INC:=-Ilibs

CFLAGS:=-Wall -std=c99 -O3
DFLAGS:=-Wall -std=c99 -DDEBUG -g
WFLAGS:=-Wall -std=c99 -O3 -s USE_SDL=2
LDLIBS:=libs/lua/liblua.a -lSDL2

all:$(BIN)

debug:CFLAGS=$(DFLAGS)
debug:all

web:CC=emcc
web:CFLAGS=$(WFLAGS)
web:all

$(BIN): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(INC) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

run:
	./$(BIN)

clean:
	rm -f $(BIN_DIR)/* $(OBJ_DIR)/*