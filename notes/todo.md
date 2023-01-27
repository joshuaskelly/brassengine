# TODO
## General
- [ ] In engine console/repl.
- [ ] Gamepad input
- [ ] DOS platform?!
- [ ] Array struct.
- [ ] Tag all structs?
- [x] Sounds.
- [x] Fix issue with freeing of dynamically allocated textures.
- [x] Audit texture module `luaL_checkinteger`. Should probably prefer `luaL_checknumber`
- [x] Rename bindings directory to modules.
- [x] Override Lua `io.open()` to sandbox/support zips. [Related SO question](https://stackoverflow.com/questions/20715652/how-to-wrap-the-io-functions-in-lua-to-prevent-the-user-from-leaving-x-directory)
- [x] Fix web build. Lua isn't linking?

## Raycaster
- [ ] Render targets
- [ ] Ceiling/floor tile data
- [x] Sprites
- [x] Depth buffer
- [x] Shading via color tables

## Console
- [ ] Split on new lines
- [ ] Remove implicit new line in log system?
- [ ] Refactor event handling. Right now the script system reaches directly into the event system and still gets event data when console is down.
- [x] Need built-in font asset
- [x] Page up/down through history. ctrl + shift + up/down
- [x] Input buffer. up / down
- [x] Support move cursor left/right + insert (memmove)
- [x] Custom prompt