local draw = require("draw")

function _init()
    black = 0
    dark_blue = 1
    dark_purple = 2
    dark_green = 3
    brown = 4
    dark_gray = 5
    light_gray = 6
    white = 7
    red = 8
    orange = 9
    yellow = 10
    green = 11
    blue = 12
    indigo = 13
    pink = 14
    peach = 15

    draw.clear(black)

    color = 0
    size = 0

    points = {}

    ramp = {0,5,4,9,10,15,7,7}
end

function _update()
    local x, y = mouse_position()
    table.insert(points, {x=x, y=y})

    if #points > 200 then
        table.remove(points, 1)
    end
end

function _draw()
    local x, y = mouse_position()
    draw.clear(0)

    if #points < 11 then
        return
    end

    for i = 1, #points - 10, 1 do
        local p0 = points[i]
        local p1 = points[i+10]
        local c = i / (#points - 10)
        draw.line(p0.x, p0.y, p1.x, p1.y, shade(c))
    end
end

function shade(t)
    local i = t * (#ramp - 1) + 1
    return ramp[i//1]
end
