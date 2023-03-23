local display = require("display")
local draw = require("draw")
local mesh = require("mesh")

-- Called once at startup
function _init()
    local scale = 8
    display.set_size(128 * scale, 128 * scale)
end

-- Called once per frame
function _update()
end

-- Called once per frame
function _draw()
    draw.clear(0)
    mesh.render()
end
