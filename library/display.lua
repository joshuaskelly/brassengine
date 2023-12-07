--- @meta

--- Module for interacting with the display.
local display = {}

--- Set display size.
--- @param width integer  Display width
--- @param height integer  Display height
function display.set_size(width, height) end

--- Set display aspect ratio.
--- @param aspect number  Pixel aspect ratio
function display.set_aspect(aspect) end

--- Set display fullscreen.
--- @param fullscreen boolean  True if to set to fullscreen, False for windowed.
function display.set_fullscreen(fullscreen) end

--- Set display title.
--- @param title string  Title to set
function display.set_title(title) end

return display