---@meta

local assets = {}

--- Get texture for given filename and frame
--- @param filename string Name of texture asset to look for
--- @param frame integer? Index of frame (default `1`)
--- @return texture Texture userdata if found, nil otherwise.
function assets.get_texture(filename, frame) end

--- Get sound for given filename
--- @param filename string Name of sound asset to look for
--- @return sound Sound userdata if found, nil otherwise.
function assets.get_sound(filename) end

return assets