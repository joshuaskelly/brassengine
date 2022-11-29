--main.lua
local engine = require("engine")

local GameScreen = require("gui.screens.game")

-- Called once at startup
function _init()
    cursor = engine.assets.get_texture("textures/gui/cursor.gif")
    cursor_down = engine.assets.get_texture("textures/gui/cursor_down.gif")
    engine.graphics.set_transparent_color(0)

    local current_room = 1

    game_screen = GameScreen(current_room)
    game_screen:move_to(1)
    current_screen = game_screen
end

local mouse_button_down = {false, false, false}
local dx, dy = 0, 0
local dragging = false

-- Called once per frame
function _update()
    local x, y = engine.input.mouse.position()

    for i = 1, 3 do
        if engine.input.mouse.button(i) then
            if not mouse_button_down[i] then
                dx, dy = x, y
            end

            if math.abs(dx - x) > 2 or math.abs(dy - y) > 2 then
                if not dragging then
                    current_screen:drag_start(dx, dy, i)
                    dragging = true
                end
            end

            mouse_button_down[i] = true

            if dragging then
                current_screen:drag(x, y, i)
            end

        elseif mouse_button_down[i] then
            mouse_button_down[i] = false

            if dragging then
                current_screen:drag_end(x, y, i)
                dragging = false
            else
                current_screen:click(x, y, i)
            end
        end
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
    if (mouse_button_down[1] or mouse_button_down[3]) and not dragging then
        engine.graphics.blit(cursor_down, x - 4, y - 2)
    else
        engine.graphics.blit(cursor, x - 4, y - 2)
    end

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
