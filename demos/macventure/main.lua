--main.lua
local json = require("json")

local assets = require("assets")
local draw = require("draw")
local graphics = require("graphics")
local mouse = require("input.mouse")

local chevyfont = require("chevyfont")
local game_data = require("gamedata")
local gui = require("gui")

-- Called once at startup
function _init()
    rt = graphics.get_render_texture()

    font = chevyfont.new("fonts/skullboy_mono/atlas8.gif", "fonts/skullboy_mono/metrics.json")
    font.size = 12

    cursor = assets.get_texture("textures/ui/cursor.gif")
    graphics.set_transparent_color(0)

    sx, sy = 0, 0

    current_room = 1

    scene_root = gui.GUI(0, 0, 320, 200)
    scene_root:add_child(gui.View(current_room))
    local frame = gui.Image("textures/ui/frame.gif", 0, 0)
    function frame:on_click(x, y)
        return false
    end
    scene_root:add_child(frame)
end

-- Called once per frame
function _update()
    if mouse.button(1) then
        local x, y = mouse.position()
        scene_root:click(x, y)
    end

    scene_root:update()
end

-- Called once per frame
function _draw()
    draw.clear(0)

    -- Draw scene
    scene_root:draw(0, 0)

    -- Draw text
    --font:draw("The last thing that you", rt, 24, 144 + 0 * font.size)
    --font:draw("remember is standing", rt, 24, 144 + 1 * font.size)
    --font:draw("before the wizard Lakmir", rt, 24, 144 + 2 * font.size)

    font:draw("INVENTORY", rt, 216, 24)

    font:draw("ENTRANCE", rt, 72, 8)

    -- Draw cursor
    local x, y = mouse.position()
    graphics.blit(cursor, x - 4, y - 2)

    --show_fps()
end

function show_fps()
    local dt = delta_time() / 1000
    local fps = 1 / dt
    local str = string.format("%.0f", fps)

    graphics.set_transparent_color(-1)
    draw.text(str, 8, 8)
    graphics.set_transparent_color(0)
end