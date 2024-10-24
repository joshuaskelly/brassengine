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

--- Returns a subtexture that share pixels with this texture.
--- @param x integer  Subtexture x-offset
--- @param y integer  Subtexture y-offset
--- @param width integer  Subtexture width
--- @param height integer  Subtexture height
--- @return texture 
function texture.texture:sub(x, y, width, height) end

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

--- Copy given source texture to this texture with given offset and size.
--- @param source texture  Texture to copy from
--- @param x integer  Destination x-offset
--- @param y integer  Destination y-offset
--- @param w integer  Destination width
--- @param h integer  Destination height
function texture.texture:blit(source, x, y, w, h) end

--- Copy given source texture to this texture with given source offset and size,
 and destination offset and size.
--- @param source texture  Texture to copy from
--- @param sx integer  Source x-offset
--- @param sy integer  Source y-offset
--- @param sw integer  Source width
--- @param sh integer  Source height
--- @param dx integer  Destination x-offset
--- @param dy integer  Destination y-offset
--- @param dw integer  Destination width
--- @param dh integer  Destination height
function texture.texture:blit(source, sx, sy, sw, sh, dx, dy, dw, dh) end

return texture