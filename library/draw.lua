--- @meta

--- Module for drawing geometric shapes.
local draw = {}

--- Draw a pixel at given position and color.
--- @param x integer  Pixel x-coordinate
--- @param y integer  Pixel y-coordinate
--- @param color integer  Pixel color
function draw.pixel(x, y, color) end

--- Draw a line between given position and color.
--- @param x0 integer  Start x-coordinate
--- @param y0 integer  Start y-coordinate
--- @param x1 integer  End x-coordinate
--- @param y1 integer  End y-coordinate
--- @param color integer  Line color
function draw.line(x0, y0, x1, y1, color) end

--- Draw line using affine texture mapping.
--- @param x0 integer  Start x-coordinate
--- @param y0 integer  Start y-coordinate
--- @param u0 number  Start 0 u-coordinate
--- @param v0 number  Start 0 v-coordinate
--- @param x1 integer  End x-coordinate
--- @param y1 integer  End y-coordinate
--- @param u1 number  End u-coordinate
--- @param v1 number  End v-coordinate
--- @param texture texture  Texture to map
function draw.textured_line(x0, y0, u0, v0, x1, y1, u1, v1, texture) end

--- Draw rectangle.
--- @param x integer  Rect top left x-coordinate
--- @param y integer  Rect top left y-coordinate
--- @param width integer  Rect width
--- @param height integer  Rect height
--- @param color integer  Line color
function draw.rectangle(x, y, width, height, color) end

--- Draw filled rectangle.
--- @param x integer  Rect top left x-coordinate
--- @param y integer  Rect top left y-coordinate
--- @param width integer  Rect width
--- @param height integer  Rect height
--- @param color integer  Fill color
function draw.filled_rectangle(x, y, width, height, color) end

--- Draw circle
--- @param x integer  Circle center x-coordinate
--- @param y integer  Circle center y-coordinate
--- @param radius integer  Circle radius
--- @param color integer  Line color
function draw.circle(x, y, radius, color) end

--- Draw filled circle
--- @param x integer  Circle center x-coordinate
--- @param y integer  Circle center y-coordinate
--- @param radius integer  Circle radius
--- @param color integer  Fill color
function draw.filled_circle(x, y, radius, color) end

--- Clear screen to given color.
--- @param color integer  Color to clear screen
function draw.clear(color) end

--- Draw text to screen.
--- @param message string  Text to draw
--- @param x integer  Text top-left x-coordinate
--- @param y integer  Text top-left y-coordinate
function draw.text(message, x, y) end

--- Draw triangle.
--- @param x0 integer  Vertex 0 x-coordinate
--- @param y0 integer  Vertex 0 y-coordinate
--- @param x1 integer  Vertex 1 x-coordinate
--- @param y1 integer  Vertex 1 y-coordinate
--- @param x2 integer  Vertex 2 x-coordinate
--- @param y2 integer  Vertex 2 y-coordinate
--- @param color integer  Line color
function draw.triangle(x0, y0, x1, y1, x2, y2, color) end

--- Draw filled triangle.
--- @param x0 integer  Vertex 0 x-coordinate
--- @param y0 integer  Vertex 0 y-coordinate
--- @param x1 integer  Vertex 1 x-coordinate
--- @param y1 integer  Vertex 1 y-coordinate
--- @param x2 integer  Vertex 2 x-coordinate
--- @param y2 integer  Vertex 2 y-coordinate
--- @param color integer  Fill color
function draw.filled_triangle(x0, y0, x1, y1, x2, y2, color) end

--- Draw triangle using affine texture mapping.
--- @param x0 integer  Vertex 0 x-coordinate
--- @param y0 integer  Vertex 0 y-coordinate
--- @param u0 number  UV 0 u-coordinate
--- @param v0 number  UV 0 v-coordinate
--- @param x1 integer  Vertex 1 x-coordinate
--- @param y1 integer  Vertex 1 y-coordinate
--- @param u1 number  UV 1 u-coordinate
--- @param v1 number  UV 1 v-coordinate
--- @param x2 integer  Vertex 2 x-coordinate
--- @param y2 integer  Vertex 2 y-coordinate
--- @param u2 number  UV 2 u-coordinate
--- @param v2 number  UV 2 v-coordinate
--- @param texture texture  Texture to map
function draw.textured_triangle(x0, y0, u0, v0, x1, y1, u1, v1, x2, y2, u2, v2, texture) end

return draw