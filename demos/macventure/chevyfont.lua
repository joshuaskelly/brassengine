local assets = require("assets")
local json = require("json")
local graphics = require("graphics")
local texture = require("graphics.texture")

local ChevyFont = {}
ChevyFont.__index = ChevyFont

function ChevyFont.new(texture, metrics_)
    local new_font = {}
    setmetatable(new_font, ChevyFont)

    new_font.texture = assets.get_texture(texture)

    f = io.open(metrics_)
    metrics = json.decode(f:read("*all"))
    io.close(f)

    new_font.size = metrics.size
    new_font.atlas = {}

    for i, v in ipairs(metrics.chars) do
        local char = metrics.chars[i]
        local x = metrics.pack_x[i]
        local y = metrics.pack_y[i]
        local w = metrics.width[i]
        local h = metrics.height[i]
        local a = metrics.advance[i]

        new_font.atlas[string.char(char)] = {
            advance = metrics.advance[i],
            offset_x = metrics.offset_x[i],
            offset_y = metrics.offset_y[i],
            width = metrics.width[i],
            height = metrics.height[i],
            pack_x = metrics.pack_x[i],
            pack_y = metrics.pack_y[i],
        }
    end

    return new_font
end

function ChevyFont:draw(text, texture_, x, y)
    local a = 0

    for i = 1, #text do
        local c = text:sub(i, i)
        local g = self.atlas[c]

        --texture.blit(self.texture, texture_, g.pack_x, g.pack_y, g.width, g.height, x + a + g.offset_x, y + 8 + g.offset_y, g.width, g.height)
        graphics.blit(self.texture, g.pack_x, g.pack_y, g.width, g.height, x + a + g.offset_x, y + 8 + g.offset_y, g.width, g.height)
        a = a + g.advance + 1
    end
end

return ChevyFont