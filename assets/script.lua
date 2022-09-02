local draw = require("draw")

-- Called once at startup
function _init()
    print("lua init")

    setup_palette()

    BUTTON_LEFT = 0
    BUTTON_RIGHT = 1
    BUTTON_UP = 2
    BUTTON_DOWN = 3
    BUTTON_A = 4
    BUTTON_B = 5

    brush_color = pink
    last_x, last_y = mouse_position()

    draw.clear(black)
end

-- Called once per frame
function _update()
    test_input()
end

-- Called once per frame
function _draw()
    test_draw()
end

function test_input()
    if button(BUTTON_LEFT) then
        draw.clear(black)
        brush_color = pink
    elseif button(BUTTON_RIGHT) then
        draw.clear(pink)
        brush_color = black
    end

    if button(BUTTON_A) then
        draw.filled_rectangle(last_x - 4, last_y - 4, 8, 8, red)
    elseif button(BUTTON_B) then
        draw.filled_rectangle(last_x - 4, last_y - 4, 8, 8, blue)
    end
end

function test_draw()
    local x, y = mouse_position()
    draw.line(last_x, last_y, x, y, brush_color)
    last_x, last_y = x, y
end

function setup_palette()
    black = 0
    dark_blue = 1
    dark_purple = 2
    dark_green = 3
    brown = 4
    dark_gray = 5
    light_ray = 6
    white = 7
    red = 8
    orange = 9
    yellow = 10
    green = 11
    blue = 12
    indigo = 13
    pink = 14
    peach = 15

    -- pico-8 palette
    palette(black, 0, 0, 0)
    palette(dark_blue, 29, 43, 83)
    palette(dark_purple, 126, 37, 83)
    palette(dark_green, 0, 135, 81)
    palette(brown, 171, 82, 54)
    palette(dark_gray, 95, 87, 79)
    palette(light_ray, 194, 195, 199)
    palette(white, 255, 241, 232)
    palette(red, 255, 0, 77)
    palette(orange, 255, 163, 0)
    palette(yellow, 255, 236, 39)
    palette(green, 0, 228, 54)
    palette(blue, 41, 173, 255)
    palette(indigo, 131, 118, 156)
    palette(pink, 255, 119, 168)
    palette(peach, 255, 204, 170)
end