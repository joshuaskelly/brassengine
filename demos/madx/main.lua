-- MADX demo

local assets = require("assets")
local display = require("display")
local draw = require("draw")
local graphics = require("graphics")
local keyboard = require("input.keyboard")
local mouse = require("input.mouse")

local Map = require("map")
local generate = require("generator")

-- Called once at startup
function _init()
    resolution = {
        width = 176,
        height = 160
    }

    --graphics.set_resolution(resolution.width, resolution.height)
    display.set_size(resolution.width * 4, resolution.height * 4)

    textures = assets.get_texture("textures.gif")

    p_x = 0
    p_y = 0

    show_grid = false

    map = Map.new(22 - 4, 20 - 4)
    generate:Noise(map)
    generate:Threshold(map, 0.85)
end

-- Called once per frame
function _update()
    local n_x, n_y = p_x, p_y

    if buttonp(0) then
        n_x = p_x - 1
    end

    if buttonp(1) then
        n_x = p_x + 1
    end

    if buttonp(2) then
        n_y = p_y - 1
    end

    if buttonp(3) then
        n_y = p_y + 1
    end

    if map:Contains(n_x, n_y) and not map:IsSolid(n_x, n_y) then
        p_x, p_y = n_x, n_y
    end
end

-- Called once per frame
function _draw()
    draw.clear(0)
    local x, y = mouse.position()

    frame("ENTRANCE")
    draw_map()
    grid()
    player()
end

function sprite(index, x, y)
    local sx = (index % 32) * 8
    local sy = (index // 32) * 8
    graphics.blit(textures, sx, sy, 8, 8, x, y, 8, 8)
end

function frame(name)
    -- Draw name

    local s = 398

    -- Draw top + bottom frame
    for x = 2, (resolution.width - 24) // 8 do
        if x > #name + 1 then
            sprite(s, x * 8, 8)
        end

        sprite(s, x * 8, resolution.height - 16)
    end

    -- Draw left + right frame
    for y = 2, (resolution.height - 24) // 8 do
        if y > 1 then
            sprite(s + 32, 8, y * 8)
        end

        sprite(s + 32, resolution.width - 16, y * 8)
    end

    -- Draw corners
    sprite(s + 62, 8, 8)
    sprite(s + 63, resolution.width - 16, 8)
    sprite(s + 63 + 31, 8, resolution.height - 16)
    sprite(s + 63 + 32, resolution.width - 16, resolution.height - 16)

    draw.text(name, 8, 8)
end

function grid()
    if not show_grid then return end

    for y = 2, (resolution.height // 8) - 3 do
        for x = 2, (resolution.width // 8) - 3 do
            draw.rectangle(x * 8, y * 8, 8, 8, 4)
        end
    end
end

function player()
    local x, y = p_x * 8 + 16, p_y * 8 + 16
    sprite(62, x, y)
end

function draw_map()
    for y = 1, map.height - 1 do
        for x = 1, map.width - 1 do
            local d = map:GetData(x, y)

            if d == 1 then
                sprite(50, x * 8 + 16, y * 8 + 16)
            end
        end
    end
end

local button_mapping = {
    [0] = 80, -- left
    [1] = 79, -- right
    [2] = 82, -- up
    [3] = 81, -- down
    [4] = 29, -- square
    [5] = 27, -- cross
}

local button_repeat = {
    [0] = 0,
    [1] = 0,
    [2] = 0,
    [3] = 0,
    [4] = 0,
    [5] = 0,
}

function buttonp(i)
    local is_button_down = keyboard.key(button_mapping[i])

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