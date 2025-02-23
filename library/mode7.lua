--- @meta

--- Module for mode7 renderer
local mode7 = {}

--- @class Renderer
mode7.Renderer = {}

--- Creates a mode7 renderer object.
--- @param render_texture texture?  Optional texture to render to. If none given, will default to global render texture.
--- @return Renderer 
function mode7.Renderer:new(render_texture) end

--- Clears color buffer.
--- @param color integer?  Color to fill buffer with.
function mode7.Renderer:clear(color) end

--- Renders given texture.
--- @param texture texture  Texture to render
--- @param callback function(integer):nil  Horizontal scanline callback. Given the scanline as integer, and does not return a value.
function mode7.Renderer:render(texture, callback) end

--- Access renderer's features.
--- @param name string  Feature name.
--- @param value any?  Value to set feature to.
function mode7.Renderer:feature(name, value) end

return mode7