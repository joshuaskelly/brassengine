--action.lua
local Action = {}
Action.__index = Action

setmetatable(Action, {
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

function Action:_init(func)
    self.target = nil
    self.func = func
    self.done = false
end

function Action:update()
    self:func()
end

return Action
