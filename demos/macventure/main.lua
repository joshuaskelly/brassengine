--main.lua
local assets = require("assets")
local draw = require("draw")
local graphics = require("graphics")
local mouse = require("input.mouse")

local GameScreen = require("gui.screens.game")

-- Called once at startup
function _init()
    cursor = assets.get_texture("textures/ui/cursor.gif")
    graphics.set_transparent_color(0)

    local current_room = 1

    game_screen = GameScreen(current_room)
    current_screen = game_screen
end

left_button_down = false

-- Called once per frame
function _update()
    if mouse.button(1) then
        left_button_down = true
    elseif left_button_down then
        left_button_down = false
        local x, y = mouse.position()
        current_screen:click(x, y)
    end

    current_screen:update()
end

-- Called once per frame
function _draw()
    draw.clear(0)

    -- Draw scene
    current_screen:draw(0, 0)

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
