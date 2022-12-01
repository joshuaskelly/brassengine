--delay.lua
local TimedAction = require("gui.actions.timedaction")

local Delay = {}
Delay.__index = Delay

setmetatable(Delay, {
    __index = TimedAction,
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

local function nop()
end

function Delay:_init(duration)
    TimedAction._init(self, nop, duration)
end

return Delay
