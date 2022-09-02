-- Called once at startup
function _init()
    print("lua init")

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
        clear(0)
        brush_color = 1
    elseif button(BUTTON_RIGHT) then
        clear(1)
        brush_color = 0
    end

    if button(BUTTON_A) then
        print("Button A")
    elseif button(BUTTON_B) then
        print("Button B")
    end
end

function test_draw()
    local x, y = mouse_position()
    draw_line(last_x, last_y, x, y, brush_color)
    last_x, last_y = x, y
end
