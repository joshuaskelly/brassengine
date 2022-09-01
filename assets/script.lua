-- Called once at startup
function _init()
    print("lua init")
    brush_color = 1
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
    if button(0) then
        clear(0)
        brush_color = 1
    elseif button(1) then
        clear(1)
        brush_color = 0
    end

    if button(4) then
        print("Button A")
    elseif button(5) then
        print("Button B")
    end
end

function test_draw()
    local x, y = mouse_position()
    draw_pixel(x, y, brush_color)
end
