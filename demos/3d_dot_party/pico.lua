--- @module pico
-- pico8-like API wrapper

local draw = require("draw")
local graphics = require("graphics")
local keyboard = require("keyboard")

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

function clip(x, y, w, h)
    graphics.set_clipping_rectangle(x, y, w, h)
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

-- Math API

local function nil_as_zero(x)
    if x == nil then
        return 0
    end

    return x
end

--- x absolute value
function abs(x)
    x = nil_as_zero(x)

    return math.abs(x)
end

--- coordinate to angle; range [0..1]
function atan2(x, y)
    x = nil_as_zero(x)
    y = nil_as_zero(y)

    return math.atan(x, y)
end

--- cosine; range [0..1]
function cos(x)
    x = nil_as_zero(x) * math.rad(360)

    return math.cos(x)
end

--- round down
function flr(x)
    x = nil_as_zero(x)

    return math.floor(x)
end

--- x/y maximum
function max(x, y)
    x = nil_as_zero(x)
    y = nil_as_zero(y)

    return math.max(x, y)
end

--- x/y/z middle value
function mid(x, y, z)
    x = nil_as_zero(x)
    y = nil_as_zero(y)
    z = nil_as_zero(z)

    if x > y and x < z then
        return x
    elseif y > x and y < z then
        return y
    end

    return z
end

--- x/y minimum
function min(x, y)
    x = nil_as_zero(x)
    y = nil_as_zero(y)

    return math.min(x, y)
end

--- random; 0 <= n < x
function rnd(x)
    if x == nil then
        x = 1
    end

    return math.random() * x
end

--- returns argument sign: -1 or 1; sgn(0) = 1
function sgn(x)
    if x >= 0 then
        return 1
    end

    return -1
end

--- x sine, [0..1]; inverted
function sin(x)
    x = nil_as_zero(x) * math.rad(360)

    return -math.sin(x)
end

--- x square root
function sqrt(x)
    x = nil_as_zero(x)

    return math.sqrt(x)
end

--- set random seed
function srand(x)
    x = nil_as_zero(x)

    math.randomseed(x)
end

-- Input API

local button_mapping = {
    [0] = 80, -- left
    [1] = 79, -- right
    [2] = 82, -- up
    [3] = 81, -- down
    [4] = 29, -- square
    [5] = 27, -- cross
}

-- get button i state for player p
function btn(i)
    return keyboard.key(button_mapping[i])
end

local button_repeat = {
    [0] = 0,
    [1] = 0,
    [2] = 0,
    [3] = 0,
    [4] = 0,
    [5] = 0,
}

-- true when the button was not pressed the last frame; delays 4 frames after button h
function btnp(i)
    local is_button_down = btn(i)

    if is_button_down then
        button_repeat[i] = button_repeat[i] + 1

        local state = button_repeat[i]

        if state == 1 or state == 16 then
            return true
        end

        if state > 16 and (state - 16) % 4 == 0 then
            return true
        end
    else
        button_repeat[i] = 0
    end

    return false
end
