local draw = require("draw")
local mouse = require("mouse")
local gifrecorder = require("gifrecorder")

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

    recorder = gifrecorder.Recorder:new()
end

function _update()
    recorder:update()
end

function _draw()
    local x, y = mouse.position()
    --draw.clear(0)
    draw.filled_circle(x, y, math.abs(math.cos(size)) * 32, color)

    color = (color + 0.1) % 16
    size = size + 000.1
end
