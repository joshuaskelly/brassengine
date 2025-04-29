--- @meta

--- Module for Mode7 renderer
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

--- @class Camera
--- @field position vector3
--- @field pitch number
--- @field yaw number
--- @field fov number
--- @field near number
--- @field far number
mode7.Camera = {}

--- Creates a mode7 camera object.
--- @param renderer Renderer 
--- @return Camera 
function mode7.Camera:new(renderer) end

return mode7