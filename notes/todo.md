# TODO
## General
- [ ] In engine console/repl.
- [ ] Gamepad input
- [ ] DOS platform?!
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