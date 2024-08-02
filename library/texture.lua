--- @meta

--- Module for working with texture data.
local texture = {}

--- Create new texture.
--- @param width integer  Texture width
--- @param height integer  Texture height
--- @return texture 
function texture.new(width, height) end

--- @class texture
--- @field pixels integer[]
--- @field width integer
--- @field height integer
texture.texture = {}

--- Returns a copy of this texture.
--- @return texture 
function texture.texture:copy() end

--- Fill entire texture with color.
--- @param color integer  Fill color
function texture.texture:clear(color) end

--- Sets pixel at given position and color.
--- @param x integer  Pixel x-coordinate
--- @param y integer  Pixel y-coordinate
--- @param color integer  Pixel color
function texture.texture:set_pixel(x, y, color) end

--- Gets pixel at given position.
--- @param x integer  Pixel x-coordinate
--- @param y integer  Pixel y-coordinate
--- @return integer Pixel color
function texture.texture:get_pixel(x, y) end

--- Copy given source texture to this texture with given offset.
--- @param source texture  Texture to copy from
--- @param x integer  Destination x-offset
--- @param y integer  Destination y-offset
function texture.texture:blit(source, x, y) end

return texture