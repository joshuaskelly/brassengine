local draw = require("draw")
local gamecontroller = require("gamecontroller")

-- Called once at startup
function _init()
end

-- Called once per frame
function _update()
end

id = 0

local function any_input(id)
    for k, v in pairs(gamecontroller.buttons) do
        if gamecontroller.button(id, v) then return true end
    end

    for k, v in pairs(gamecontroller.axes) do
        if gamecontroller.axis(id, v) > 0.5 then return true end
    end

    return false
end

-- Called once per frame
function _draw()
    draw.clear(0)
    
    local connected = gamecontroller.connected()
    if #connected == 0 then
        draw.text("no controllers detected", 68, 92)
        return
    end

    draw.text("controller: "..id, 0, 0)
    draw.text(" buttons", 0, 8)

    local y = 16

    for _, i in ipairs(connected) do
        if any_input(i) then id = i end
    end

    for k, v in pairs(gamecontroller.buttons) do
        draw.text("  "..k..": "..(gamecontroller.button(id, v) and 1 or 0), 0, y)
        y = y + 8
    end

    draw.text(" axes", 160, 8)
    y = 16
    for k, v in pairs(gamecontroller.axes) do
        draw.text("  "..k..": "..gamecontroller.axis(id, v), 162, y)
        y = y + 8
    end
end
