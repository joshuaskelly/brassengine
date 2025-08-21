# Build

## Dependencies

Platforms using SDL have a min required version of 2.26.0

## Profiling

1. Add `-pg -no-pie` to DFLAGS
2. Make debug build
3. Run brass
4. Run `gprof ./build/bin/brass > out.text` to get profile info

## Finding Memory Leaks (MacOS)
1. Make debug build
2. `$ leaks -quiet --atExit -- ./build/bin/brass path/to/assets`

## Notes

When building on Windows with mingw64 + msys you need to build Lua via:

`make a CFLAGS+=-DLUA_USE_LONGJUMP`

When building on Mac ARM clang 15.0.0+ is required to build lua