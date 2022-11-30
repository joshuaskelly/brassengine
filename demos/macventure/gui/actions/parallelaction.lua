--sequentialaction.lua
local Action = require("gui.action")

local ParallelAction = {}
ParallelAction.__index = ParallelAction

setmetatable(ParallelAction, {
    __index = Action,
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

local function nop()
end

function ParallelAction:_init(...)
    Action._init(self, nop)

    self.actions = table.pack(...)
end

function ParallelAction:update()
    self.done = true

    for _, action in ipairs(self.actions) do
        if not action.done then
            action:update()
        end

        self.done = self.done and action.done
    end
end

return ParallelAction
