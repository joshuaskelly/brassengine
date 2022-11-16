local assets = require("assets")
local graphics = require("graphics")
local texture = require("graphics.texture")

local GUI = require("gui")

local Image = {}
Image.__index = Image

setmetatable(Image, {
    __index = GUI,
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

function Image:_init(texture_name, x, y)
    local t = assets.get_texture(texture_name)
    local width, height = t:size()

    GUI._init(self, x, y, width, height)

    self.texture = t
end

function Image:draw(offset_x, offset_y)
    graphics.blit(self.texture, offset_x + self.rect.x, offset_y + self.rect.y)
    GUI.draw(self, offset_x, offset_y)
end

function Image:on_click(x, y, button)
    return false
end

function Image:contains(x, y)
    local color = texture.get_pixel(self.texture, x - self.rect.x, y - self.rect.y)
    return color ~= 0
end

return Image
