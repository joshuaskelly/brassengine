local chevyfont = require("chevyfont")

local GUI = require("gui")

local font = chevyfont.new("fonts/skullboy_mono/atlas8.gif", "fonts/skullboy_mono/metrics.json")
font.size = 12

local Text = {}
Text.__index = Text

setmetatable(Text, {
    __index = GUI,
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

function Text:_init(text, x, y)
    local width = (#text) * 8
    local height = 8

    GUI._init(self, x, y, width, height)

    self.text = text
end

function Text:draw(offset_x, offset_y)
    font:draw(self.text, offset_x + self.rect.x, offset_y + self.rect.y)
    GUI.draw(self, offset_x, offset_y)
end

function Text:set(text)
    self.text = text
    self.rect.width = (#text) * 8
end

return Text
