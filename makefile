CC:=gcc
BIN:=toy

SRC_DIR:=src
BUILD_DIR:=build
OBJ_DIR:=$(BUILD_DIR)/obj
BIN_DIR:=$(BUILD_DIR)/bin
WEB_DIR:=$(BUILD_DIR)/web

BIN:=$(BIN_DIR)/$(BIN)
SRCS:=$(wildcard $(SRC_DIR)/*.c)
OBJS:= $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
INC:=-Ilibs

LIBLUA:=libs/lua/liblua.a

CFLAGS:=-Wall -std=c99 -O3
DFLAGS:=-Wall -std=c99 -DDEBUG -g
LFLAGS:=
LDLIBS:=$(LIBLUA) -lSDL2

all:$(BIN)

debug:CFLAGS=$(DFLAGS)
debug:all

web:CC=emcc -s USE_SDL=2
web:LFLAGS=AR='emar rcu' RANLIB=emranlib
web: $(OBJS) | $(BIN_DIR) $(LIBLUA)
	$(CC) $^ $(LIBLUA) -o $(WEB_DIR)/main.html

$(BIN): $(OBJS) | $(BIN_DIR) $(LIBLUA)
	$(CC) $(CFLAGS) $(INC) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

$(LIBLUA):
	cd libs/lua && make a CC=$(CC) $(LFLAGS)

run:
	./$(BIN)

serve:
	emrun $(WEB_DIR)/main.html

clean:
	rm -f $(BIN_DIR)/* $(OBJ_DIR)/* $(WEB_DIR)/*
	cd libs/lua && make clean
