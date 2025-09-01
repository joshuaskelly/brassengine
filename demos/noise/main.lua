local draw = require("draw")
local graphics = require("graphics")
local texture = require("texture")

function _init ()
    t = texture.new(320, 200)
    offset = {
        x = math.random(-10000, 10000) + math.random(),
        y = math.random(-10000, 10000) + math.random()
    }

    pixels = {}
    freq = math.random(4, 20) + math.random()

    for y = 1, 200 do
        for x = 1, 320 do
            local p = (math.noise(x / 200 * freq + offset.x, y / 200 * freq + offset.y) + 1) / 2 * 255

            if p > 118 and p < 132 then
                --p = 255
            else
                --p = 0
            end

            table.insert(pixels, p)
        end
    end

    t.pixels = pixels
end

function _draw()
    draw.clear(0)
    draw.texture(t, 0, 0)
end
