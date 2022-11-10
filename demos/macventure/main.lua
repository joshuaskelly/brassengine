--main.lua
local engine = require("engine")

local GameScreen = require("gui.screens.game")

-- Called once at startup
function _init()
    cursor = engine.assets.get_texture("textures/ui/cursor.gif")
    engine.graphics.set_transparent_color(0)

    local current_room = 1

    game_screen = GameScreen(current_room)
    current_screen = game_screen
end

left_button_down = false

-- Called once per frame
function _update()
    if engine.input.mouse.button(1) then
        left_button_down = true
    elseif left_button_down then
        left_button_down = false
        local x, y = engine.input.mouse.position()
        current_screen:click(x, y)
    end

    current_screen:update()
end

-- Called once per frame
function _draw()
    engine.draw.clear(0)

    -- Draw scene
    current_screen:draw(0, 0)

    -- Draw cursor
    local x, y = engine.input.mouse.position()
    engine.graphics.blit(cursor, x - 4, y - 2)

    --show_fps()
end

function show_fps()
    local dt = delta_time() / 1000
    local fps = 1 / dt
    local str = string.format("%.0f", fps)

    engine.graphics.set_transparent_color(-1)
    engine.draw.text(str, 8, 8)
    engine.graphics.set_transparent_color(0)
end
