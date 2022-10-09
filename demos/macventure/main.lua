local assets = require("assets")
local draw = require("draw")
local graphics = require("graphics")
local mouse = require("input.mouse")
local chevyfont = require("chevyfont")

-- Called once at startup
function _init()
    rt = graphics.get_render_texture()
    --font = chevyfont.new("fonts/pinch/atlas8.gif", "fonts/pinch/metrics.json")
    font = chevyfont.new("fonts/skullboy_mono/atlas8.gif", "fonts/skullboy_mono/metrics.json")
    font.size = 12
    --font = chevyfont.new("fonts/thicket_mono/atlas8.gif", "fonts/thicket_mono/metrics.json")

    scene = assets.get_texture("entrance.gif")
    skull = assets.get_texture("skull.gif")
    frame = assets.get_texture("frame.gif")
    cursor = assets.get_texture("cursor.gif")
    graphics.set_transparent_color(0)

    sx, sy = 0, 0
end

-- Called once per frame
function _update()
end

-- Called once per frame
function _draw()
    draw.clear(0)

    local x, y = mouse.position()

    graphics.set_clipping_rectangle(16, 16, 176, 112)

    if x > 16 and x < 192 and y > 16 and y < 128 then
        sx = math.floor((104 - x) / 88 * 16)
        --sy = math.floor((72 - y) / 56 * 16) + 16
    end

    graphics.blit(scene, 0, 0, 208, 128, sx, sy, 208, 128)
    -- 94, 34
    graphics.blit(skull, 0, 0, 19, 22, 94 + sx, 34 + sy, 19, 22)

    graphics.set_clipping_rectangle(0, 0, 320, 200)


    font:draw("The last thing that you", rt, 24, 144 + 0 * font.size)
    font:draw("remember is standing", rt, 24, 144 + 1 * font.size)
    font:draw("before the wizard Lakmir", rt, 24, 144 + 2 * font.size)

    font:draw("INVENTORY", rt, 216, 24)


    graphics.blit(frame, 0, 0, 320, 200, 0, 0, 320, 200)
    graphics.blit(cursor, 0, 0, 16, 16, x - 4, y, 16, 16)
end
