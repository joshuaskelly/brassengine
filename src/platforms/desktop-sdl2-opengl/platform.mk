# SDl2 + OpenGL specific variables and targets

LDLIBS+=`sdl2-config --libs` -lSDL2_mixer

# OpenGL libs
ifeq ($(OS),Windows_NT)
LDLIBS+=-lglew32 -lopengl32
else
LDLIBS+=-lGLEW -lGL
endif

PLATFORM_OBJ_DIR=$(OBJ_DIR)/platforms/$(PLATFORM)
PLATFORM_SRC_DIR=$(SRC_DIR)/platforms/$(PLATFORM)

# Platform specific platform.o target
$(PLATFORM_OBJ_DIR)/platform.o: $(PLATFORM_SRC_DIR)/platform.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INC) `sdl2-config --cflags` -c $< -o $@
