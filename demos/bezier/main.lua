local draw = require("draw")
local assets = require("assets")
local graphics = require("graphics")
local keyboard = require("keyboard")
local math = require("math")
local mouse = require("mouse")
local vector2 = require("vector2")

local cursor = assets.get_texture("cursor.gif")
local pattern = assets.get_texture("pattern.gif")

local points = {
    vector2.new(80, 160),
    vector2.new(60, 40),
    vector2.new(260, 160),
    vector2.new(240, 40)
}

local active = nil

function _init()
    graphics.set_transparent_color(7)
end

function _update()
    if mouse.button(1) then
        if not active then
            -- Pick control point if none active
            local x, y = mouse.position()
            local m = vector2.new(x, y)

            for _, p in ipairs(points) do
                if vector2.distance(m, p) <= 2 then
                    active = p
                    goto done
                end
            end
            ::done::
        else
            -- Drag active control point
            local x, y = mouse.position()

            -- Snap position if ctrl held
            if keyboard.key(224) or keyboard.key(228) then
                x = (x + 5) // 10 * 10
                y = (y + 5) // 10 * 10
            end

            active.x, active.y = x, y
        end
    else
        -- Deselect when button released
        active = nil
    end
end

function _draw()
    -- Background
    draw.filled_rectangle(0, 0, 320, 200, pattern)

    -- Info
    draw.text("bezier curve demo", 0, 0)
    draw.text("click + drag dots", 0, 182)
    draw.text("hold ctrl to snap", 0, 192)

    draw.line(points[1].x, points[1].y, points[2].x, points[2].y, 2)
    draw.line(points[3].x, points[3].y, points[4].x, points[4].y, 2)

    -- Handles`
    for _, point in ipairs(points) do
        draw.filled_circle(point.x, point.y, 2, 2)
    end

    local x, y = mouse.position()

    -- Hover
    for _, point in ipairs(points) do
        if dist(x, y, point.x, point.y) < 2 then
            draw.filled_circle(point.x, point.y, 2, 3)
            goto done
        end
    end
    ::done::

    if active then
        draw.filled_circle(active.x, active.y, 2, 4)
    end

    -- Curve
    draw.bezier(
        points[1].x, points[1].y,
        points[2].x, points[2].y,
        points[3].x, points[3].y,
        points[4].x, points[4].y,
        4
    )

    -- Cursor
    graphics.blit(cursor, x, y)
end

function dist(x0, y0, x1, y1)
    local x = x1 - x0
    local y = y1 - y0
    return math.sqrt(x * x + y * y)
end
