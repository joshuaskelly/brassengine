--- @module pico
-- pico8-like API wrapper

local draw = require("draw")

-- Graphics API

local default_color = 6
local draw_color = default_color

local function color_or_default(col)
     if col == nil then
         return draw_color
     end

     return col
end

--- draw circle
function circ(x, y, r, col)
    draw.circle(x, y, r, color_or_default(col))
end

--- draw filled circle
function circfill(x, y, r, col)
    draw.filled_circle(x, y, r, color_or_default(col))
end

function clip()
end

--- clear screen; col = clear color
function cls(col)
    if col == nil then
        draw.clear(0)
    else
        draw.clear(col)
    end
end

--- set default color
function color(col)
    if col == nil then
        draw_color = default_color
    else
        draw_color = col
    end
end

--- draw line
function line(x0, y0, x1, y1, col)
    draw.line(x0, y0, x1, y1, color_or_default(col))
end

function pget()
end

function pset()
end

--- draw rectangle
function rect(x0, y0, x1, y1, col)
    local w = x1 - x0
    local h = y1 - y0
    draw.rectangle(x0, y0, w, h, color_or_default(col))
end

--- draw filled rectangle
function rectfill(x0, y0, x1, y1, col)
    local w = x1 - x0
    local h = y1 - y0
    draw.filled_rectangle(x0, y0, w, h, color_or_default(col))
end

-- Table API

--- add v to t
function add(t, v)
    table.insert(t, v)
end

-- used in 'for v in all(t)' loops
function all(t)
    local i = 0
    local n = #t
    return function()
        i = i + 1
        if i <=n then
            return t[i]
        end
    end
end
