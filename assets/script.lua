-- Called once at startup
function _init()
    print("lua init")
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
        print("left")
    elseif button(1) then
        print("right")
    end
end

function test_draw()
    local x, y = mouse_position()
    draw_pixel(x, y, 1)
end
