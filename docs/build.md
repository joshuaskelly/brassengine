# Build

### Notes

When building on Windows with mingw64 + msys you need to build Lua via:

`make a CFLAGS+=-DLUA_USE_LONGJUMP`