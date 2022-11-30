--timedaction.lua
local Action = require("gui.action")

local TimedAction = {}
TimedAction.__index = TimedAction

setmetatable(TimedAction, {
    __index = Action,
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

function TimedAction:_init(func, duration)
    Action._init(self, func)

    self.start = nil
    self.duration = duration
    self.progress = 0
end

function TimedAction:update()
    if self.start == nil then
        self.start = time()
    end
    
    self.progress = math.min((time() - self.start) / self.duration, 1)

    Action.update(self)

    if self.progress == 1 then
        self.done = true
    end
end

return TimedAction
