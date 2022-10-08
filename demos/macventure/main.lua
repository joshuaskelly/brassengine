local draw = require("draw")
local graphics = require("graphics")
local chevyfont = require("chevyfont")

-- Called once at startup
function _init()
    rt = graphics.get_render_texture()
    --font = chevyfont.new("fonts/pinch/atlas8.gif", "fonts/pinch/metrics.json")
    font = chevyfont.new("fonts/skullboy_mono/atlas8.gif", "fonts/skullboy_mono/metrics.json")
    font.size = 12
    --font = chevyfont.new("fonts/thicket_mono/atlas8.gif", "fonts/thicket_mono/metrics.json")
end

-- Called once per frame
function _update()
end

-- Called once per frame
function _draw()
    draw.clear(0)

    font:draw("The last thing that you", rt, 0, 0 * font.size)
    font:draw("remember is standing", rt, 0, 1 * font.size)
    font:draw("before the wizard Lakmir", rt, 0, 2 * font.size)
end
