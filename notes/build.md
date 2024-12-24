# Build

## Profiling

1. Add `-pg -no-pie` to DFLAGS
2. Make debug build
3. Run brass
4. Run `gprof ./build/bin/brass > out.text` to get profile info

## Notes

When building on Windows with mingw64 + msys you need to build Lua via:

`make a CFLAGS+=-DLUA_USE_LONGJUMP`

When building on Mac ARM clang 15.0.0+ is required to build lua