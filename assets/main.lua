local draw = require("draw")
local gamecontroller = require("gamecontroller")

-- Called once at startup
function _init()
end

-- Called once per frame
function _update()
end

-- Called once per frame
function _draw()
    draw.clear(0)
    draw.text("controller", 0, 0)
    draw.text(" buttons", 0, 8)

    local y = 16

    for k, v in pairs(gamecontroller.buttons) do
        draw.text("  "..k..": "..(gamecontroller.button(v) and 1 or 0), 0, y)
        y = y + 8
    end

    draw.text(" axes", 160, 8)
    y = 16
    for k, v in pairs(gamecontroller.axes) do
        draw.text("  "..k..": "..gamecontroller.axis(v), 162, y)
        y = y + 8
    end

    -- draw.text("  a: "..(gamecontroller.button(0) and 1 or 0), 0, 16)
    -- draw.text("  b: "..(gamecontroller.button(1) and 1 or 0), 0, 24)
    -- draw.text("  x: "..(gamecontroller.button(2) and 1 or 0), 0, 32)
    -- draw.text("  y: "..(gamecontroller.button(3) and 1 or 0), 0, 40)
    -- draw.text(" axes", 0, 48)
    -- draw.text("  0: "..gamecontroller.axis(0), 0, 56)
    -- draw.text("  1: "..gamecontroller.axis(1), 0, 64)
    -- draw.text("  2: "..gamecontroller.axis(2), 0, 72)
    -- draw.text("  3: "..gamecontroller.axis(3), 0, 80)
end
