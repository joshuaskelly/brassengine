local gamedata = require("gamedata")
local GUI = require("gui")
local Image = require("gui.image")
local Text = require("gui.text")

local DescriptionText = {}
DescriptionText.__index = DescriptionText

setmetatable(DescriptionText, {
    __index = GUI,
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

function DescriptionText:_init(x, y)
    GUI._init(self, x, y, 224, 40)

    local t = Text("hello world\nhello world", 0, 0)
    self:add_child(t)
end

return DescriptionText