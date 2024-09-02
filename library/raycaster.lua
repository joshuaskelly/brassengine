--- @meta

--- Module for raycaster renderer
local raycaster = {}

--- @class Map
--- @field walls integer[]
--- @field floors integer[]
--- @field ceilings integer[]
raycaster.Map = {}

--- Create a new map.
--- @param width integer  Map width
--- @param height integer  Map height
--- @return Map 
function raycaster.Map.new(width, height) end

--- @class Renderer
raycaster.Renderer = {}

--- Creates a raycaster renderer object.
--- @return Renderer 
function raycaster.Renderer:new() end

--- Clears internal buffers.
--- @param target string?  One of: "all" (default), "color", or "depth".
function raycaster.Renderer:clear(target) end

--- Renders given map.
--- @param map Map  Map to render.
--- @param tiles texture[]  An array of textures. Valid range of indices is 0-255.
function raycaster.Renderer:render(map, tiles) end

--- Renders given sprite.
--- @param sprite texture  Sprite to render.
--- @param position vector2  Position of sprite.
function raycaster.Renderer:render(sprite, position) end

--- Renders given sprite with orientation.
--- @param sprite texture  Sprite to render.
--- @param position vector2  Position of sprite.
--- @param forward vector2  Forward vector of sprite.
function raycaster.Renderer:render(sprite, position, forward) end

--- Set renderer's camera data.
--- @param position vector2  Camera position.
--- @param direction vector2  Camera forward vector.
--- @param fov number  Camera field of view in degrees.
function raycaster.Renderer:camera(position, direction, fov) end

--- Access renderer's features.
--- @param name string  Feature name.
--- @param value any?  Value to set feature to.
function raycaster.Renderer:feature(name, value) end

return raycaster