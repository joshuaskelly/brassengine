ifeq ($(OS),Windows_NT)
XLIBS=-lglew32 -lopengl32
else
XLIBS=-lGLEW -lGL
endif
