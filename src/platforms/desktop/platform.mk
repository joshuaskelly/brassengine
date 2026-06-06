# SDl2 specific variables and targets

LDLIBS+=`sdl2-config --libs` -lSDL2_mixer

# Platform specific platform.o target
$(OBJ_DIR)/platform.o: $(SRC_DIR)/platform.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INC) `sdl2-config --cflags` -c $< -o $@
