# Emscripten specific variables and targets

# Tools
CC=emcc -s USE_SDL=2 -s USE_SDL_MIXER=2 -s USE_GIFLIB=1 -s FULL_ES2=1 -s EXPORTED_FUNCTIONS=_main,_free
AR='emar rcu'
RANLIB=emranlib

# Special case libs because Emscripten has giflib builtin
LIBS=$(LIBLUA) $(LIBZIP) $(LIBCJSON) $(LIBMATHC)

web-sdl2: $(OBJS) | $(BIN_DIR) $(LIBS)
	$(CC) $^ $(LIBS) -o $(WEB_DIR)/index.html --embed-file assets
