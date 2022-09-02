local draw = require("draw")

-- Called once at startup
function _init()
    print("lua init")

    -- pico-8 palette
    palette( 0, 0, 0, 0)       -- black
    palette( 1, 29, 43, 83)    -- dark-blue
    palette( 2, 126, 37, 83)   -- dark-purple
    palette( 3, 0, 135, 81)    -- dark-green
    palette( 4, 171, 82, 54)   -- brown
    palette( 5, 95, 87, 79)    -- dark-gray
    palette( 6, 194, 195, 199) -- light-gray
    palette( 7, 255, 241, 232) -- white
    palette( 8, 255, 0, 77)    -- red
    palette( 9, 255, 163, 0)   -- orange
    palette(10, 255, 236, 39)  -- yellow
    palette(11, 0, 228, 54)    -- green
    palette(12, 41, 173, 255)  -- blue
    palette(13, 131, 118, 156) -- indigo
    palette(14, 255, 119, 168) -- pink
    palette(15, 255, 204, 170) -- peach

    brush_color = 1
    last_x, last_y = mouse_position()
end

-- Called once per frame
function _update()
    test_input()
end

-- Called once per frame
function _draw()
   test_draw()
end

BUTTON_LEFT = 0
BUTTON_RIGHT = 1
BUTTON_UP = 2
BUTTON_DOWN = 3
BUTTON_A = 4
BUTTON_B = 5

function test_input()
    if button(BUTTON_LEFT) then
        draw.clear(0)
        brush_color = 1
    elseif button(BUTTON_RIGHT) then
        draw.clear(1)
        brush_color = 0
    end

    if button(BUTTON_A) then
        print("Button A")
        draw.filled_rectangle(last_x - 4, last_y - 4, 8, 8, 3)
    elseif button(BUTTON_B) then
        print("Button B")
        draw.filled_rectangle(last_x - 4, last_y - 4, 8, 8, 2)
    end
end

function test_draw()
    local x, y = mouse_position()
    draw.line(last_x, last_y, x, y, brush_color)
    last_x, last_y = x, y
end
