# Notes

## References
- [Lua 5.4 Reference Manual](https://www.lua.org/manual/5.4/manual.html)
- [DOOM source code](https://github.com/id-Software/DOOM)
- [Lua Style Guide](https://github.com/luarocks/lua-style-guide)
- [Voxel Space Engine GitHub](https://github.com/s-macke/VoxelSpace)

## Mode 7
- [Tonc: Mode 7 Part 1](http://www.coranac.com/tonc/text/mode7.htm)
- [ModeMode 7 in Lite C](https://fenixfox-studios.com/content/mode_7/)
- [Kulor's Guide to Mode 7 Perspective Planes](https://forums.nesdev.org/viewtopic.php?t=24053)
- [Mode 7 Previewer](https://novasquirrel.github.io/Mode7Preview/)

## GIFLIB Basic Example
- [gif-lib basic example](https://gist.github.com/suzumura-ss/a5e922994513e44226d33c3a0c2c60d1)

## Sandboxing Lua io.open()
- [Stack Overflow Question](https://stackoverflow.com/questions/20715652/how-to-wrap-the-io-functions-in-lua-to-prevent-the-user-from-leaving-x-directory)

## Line Rasterization
- [How to Draw Ugly Lines Really Fast](https://cohost.org/tomforsyth/post/648716-how-to-draw-ugly-lin)
- [Drawing Thick Lines](http://kt8216.unixcab.org/murphy/index.html)
- [The Beauty of Bresenham's Algorithm](https://zingl.github.io/bresenham.html)

## Audio Production
- Ensure the following for Audacity
  - Project rate 11025Hz
  - Disable resample dither
  - Resample track to 11025
  - Save as unsigned 8bit PCM

- Audio conversion with FFMPEG
  - `ffmpeg -i input.mp3 -vn -acodec pcm_u8 -ar 11025 -ac 1 output.wav`
