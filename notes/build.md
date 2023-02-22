# Build

### Notes

When building on Windows with mingw64 + msys you need to build Lua via:

`make a CFLAGS+=-DLUA_USE_LONGJUMP`

When building on Mac ARM clang 15.0.0+ is required to build lua