local draw = require("draw")
local pico = require("apis/pico")

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
    fill_color = black
    circle_color = 0
    last_x, last_y = mouse_position()

    --draw_frame()
end

-- Called once per frame
function _update()
    --test_input()
end

-- Called once per frame
function _draw()
    --test_draw()
    --test_circle()
    test_pico()
end

function test_input()
    if button(BUTTON_LEFT) then
        brush_color = pink
        fill_color = black
        draw_frame()
    elseif button(BUTTON_RIGHT) then
        brush_color = black
        fill_color = pink
        draw_frame()
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

function draw_frame()
    draw.clear(brush_color)
    clip(0, 0, 320, 200)
    draw.filled_rectangle(48, 48, 224, 104, white)
    draw.filled_rectangle(50, 50, 220, 100, fill_color)
    clip(50, 50, 220, 100)
end

function test_circle()
    draw.clear(white)
    local x, y = mouse_position()
    --draw.circle(x, y, math.min(x, y), white)
    pico.color(circle_color)
    pico.circ(x, y, math.min(x, y), circle_color)
    pico.circfill(x, y, x / 20)

    circle_color = (circle_color + 1) % 16
end

function test_pico()
    --pico.cls(black)
    pico.clip(50, 50, 220, 100)
    local x, y = mouse_position()
    --pico.circ(x, y, 16, pink)

    pico.rectfill(x - 4, y - 4, 8, 8, circle_color)
    --p.draw()

    circle_color = (circle_color + 1) % 16
end
