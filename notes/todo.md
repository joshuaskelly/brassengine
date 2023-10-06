# TODO
## General
- [ ] Gamepad input
- [ ] Support for loading all frames from GIF files.
- [ ] Support for creating/modifiying sound objects.
- [ ] Override io.lines() to sandbox/support zips.
- [ ] Default palette?
- [ ] DOS platform?!
- [ ] Dreamcast platform?!
- [ ] Array struct.
- [x] Error on missing main.lua instead of segfaulting.
- [x] Audit loadbuffer usage for filename correctness.
- [x] Move reload to core and add Lua binding
- [x] In engine console/repl.
- [x] Sounds.
- [x] Fix issue with freeing of dynamically allocated textures.
- [x] Audit texture module `luaL_checkinteger`. Should probably prefer `luaL_checknumber`
- [x] Rename bindings directory to modules.
- [x] Override Lua `io.open()` to sandbox/support zips. [Related SO question](https://stackoverflow.com/questions/20715652/how-to-wrap-the-io-functions-in-lua-to-prevent-the-user-from-leaving-x-directory)
- [x] Fix web build. Lua isn't linking?

## Raycaster
- [ ] Direction aligned sprites
- [ ] Pixels per unit feauture to support tall/short walls/sprites.
- [ ] Investigate sprite perf.
- [x] Render targets
- [x] Ceiling/floor tile data
- [x] Depth buffer? Work on sprite perf.
- [x] Sprites
- [x] Depth buffer
- [x] Shading via color tables

## Console
- [ ] Tab completion?
- [ ] Remove implicit new line in log system?
- [ ] Refactor event handling. Right now the script system reaches directly into the input system and still gets event data when console is down.
- [x] Fix errors being output twice.
- [x] Split on new lines
- [x] Need built-in font asset
- [x] Page up/down through history. ctrl + shift + up/down
- [x] Input buffer. up / down
- [x] Support move cursor left/right + insert (memmove)
- [x] Custom prompt