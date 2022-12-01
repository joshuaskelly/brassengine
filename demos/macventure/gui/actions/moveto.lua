--moveto.lua
local TimedAction = require("gui.actions.timedaction")

local MoveTo = {}
MoveTo.__index = MoveTo

setmetatable(MoveTo, {
    __index = TimedAction,
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

local function lerp(a, b, t)
    return a * (1 - t) + b * t
end

local function move(self)
    if self.origin == nil then
        self.origin = {x = self.target.rect.x, y = self.target.rect.y}
    end

    self.target.rect.x = math.floor(lerp(self.origin.x, self.destination.x, self.progress))
    self.target.rect.y = math.floor(lerp(self.origin.y, self.destination.y, self.progress))
end

function MoveTo:_init(x, y, duration)
    TimedAction._init(self, move, duration)
    self.destination = {x = x, y = y}
    self.origin = nil
end

return MoveTo
