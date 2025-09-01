--- @meta

--- Module for drawing to render texture.
local graphics = {}

--- Draw a pixel at given position and color.
--- @param x integer  Pixel x-coordinate
--- @param y integer  Pixel y-coordinate
--- @param color integer  Pixel color
function graphics.set_pixel(x, y, color) end

--- Blit given texture to render texture.
--- @param texture texture  Source texture to blit
--- @param dx integer  Destination x-offset
--- @param dy integer  Destination y-offset
function graphics.blit(texture, dx, dy) end

--- Blit given texture to render texture.
--- @param texture texture  Source texture to blit
--- @param sx integer  Source x-offset
--- @param sy integer  Source y-offset
--- @param sw integer  Source width
--- @param sh integer  Source height
--- @param dx integer  Destination x-offset
--- @param dy integer  Destination y-offset
--- @param dw integer  Destination width
--- @param dh integer  Destination height
function graphics.blit(texture, sx, sy, sw, sh, dx, dy, dw, dh) end

--- Sets clipping rectangle which defines drawable area.
--- @param x integer  Rect top left x-coordinate
--- @param y integer  Rect top left y-coordinate
--- @param width integer  Rect width
--- @param height integer  Rect height
function graphics.set_clipping_rectangle(x, y, width, height) end

--- Gets render texture.
--- @return texture Render texture userdata.
function graphics.get_render_texture() end

--- Set color for graphics palette.
--- @param index integer  Index of color to set
--- @param r integer  Red value. [0, 255]
--- @param g integer  Green value. [0, 255]
--- @param b integer  Blue value. [0, 255]
function graphics.set_global_palette_color(index, r, g, b) end

--- Set render buffer resolution.
--- @param width integer  Resolution width
--- @param height integer  Resolution height
function graphics.set_resolution(width, height) end

--- Get render buffer resolution.
--- @return integer Resolution width
--- @return integer Resolution height
function graphics.get_resolution() end

return graphics