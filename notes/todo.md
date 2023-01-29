# TODO
## General
- [ ] Gamepad input
- [ ] DOS platform?!
- [ ] Array struct.
- [ ] Tag all structs?
- [x] Move reload to core and add Lua binding
- [x] In engine console/repl.
- [x] Sounds.
- [x] Fix issue with freeing of dynamically allocated textures.
- [x] Audit texture module `luaL_checkinteger`. Should probably prefer `luaL_checknumber`
- [x] Rename bindings directory to modules.
- [x] Override Lua `io.open()` to sandbox/support zips. [Related SO question](https://stackoverflow.com/questions/20715652/how-to-wrap-the-io-functions-in-lua-to-prevent-the-user-from-leaving-x-directory)
- [x] Fix web build. Lua isn't linking?

## Raycaster
- [ ] Render targets
- [ ] Ceiling/floor tile data
- [ ] Depth buffer? Work on sprite perf.
- [x] Sprites
- [x] Depth buffer
- [x] Shading via color tables

## Console
- [ ] Remove implicit new line in log system?
- [ ] Refactor event handling. Right now the script system reaches directly into the input system and still gets event data when console is down.
- [x] Split on new lines
- [x] Need built-in font asset
- [x] Page up/down through history. ctrl + shift + up/down
- [x] Input buffer. up / down
- [x] Support move cursor left/right + insert (memmove)
- [x] Custom prompt