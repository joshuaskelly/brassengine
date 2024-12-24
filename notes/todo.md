# TODO
## General
- [ ] Fix "BAD!" errors
- [ ] Simplify web html wrapper
- [ ] Polygon rasterization for draw module
- [ ] Event based input?
- [ ] Override io.lines() to sandbox/support zips.
- [ ] Default palette?
- [ ] iOS platform
- [ ] DOS platform?!
- [ ] Dreamcast platform?!
- [ ] Threaded rendering
- [x] Gamepad input
- [x] Add foreground/background color as optional args to draw text.
- [x] Platform specific Lua module
- [x] Audit custom clamp() implementations
- [x] Audit/fix draw rectangle functions. Width/height seems off?
- [x] Support for loading all frames from GIF files.
- [x] Support for creating/modifiying sound objects.
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
- [ ] ~~Array struct.~~

## Raycaster Renderer
- [ ] Improved demo assets
- [ ] Support vertical offsets for camera.
- [ ] Investigate sprite perf.
- [ ] Clean up some workaround globals.
- [ ] Ensure rendering pixel correctness. Position camera such that walls/sprites are exactly 32x32, 64x64, 128x128, and 256x256 in screen space.
- [x] Pixels per unit feauture to support tall/short walls/sprites.
- [x] Support vertical offsets for entities.
- [x] Direction aligned sprites
- [x] Render targets
- [x] Ceiling/floor tile data
- [x] Depth buffer? Work on sprite perf.
- [x] Sprites
- [x] Depth buffer
- [x] Shading via color tables

## Mode7 Renderer
- [ ] Initial implementation

## VoxelSpace Renderer
- [ ] Initial implementation

## Sector (DOOM/Build style) Renderer
- [ ] Initial implementation

## OpenGL Platform
- [ ] Indexed shader. Send texture data as unsigned bytes, and the palette is RGB.

## Console
- [ ] Word navigation (ctrl + key) Needs to be alt + arrow on Mac 😫
  - [ ] Left/right
  - [ ] Delete/backspace
- [ ] Support for custom console background. Custom draw callback?
- [x] Tab completion?
- [x] Ensure console colors + clear colors are correct before draw
- [x] Refactor event handling. Right now the script system reaches directly into the input system and still gets event data when console is down.
- [x] Fix errors being output twice.
- [x] Split on new lines
- [x] Need built-in font asset
- [x] Page up/down through history. ctrl + shift + up/down
- [x] Input buffer. up / down
- [x] Support move cursor left/right + insert (memmove)
- [x] Custom prompt
- [ ] ~~Remove implicit new line in log system?~~

## Sound
- [ ] Streaming audio support?
- [ ] Remove SDL Mixer dependency?

## Documentation
- [ ] Fix how Language Server definition script treats global functions. (fix globals.lua and math.lua)
- [x] Restructure vector2/3/4 to reflect OO methods (like matrix/quaternion)
- [x] Prefer degrees/radians in place of angle/fov
- [x] Types for parameters and return values.
- [ ] ~~Prefer amount over t~~
