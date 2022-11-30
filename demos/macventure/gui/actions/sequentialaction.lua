--sequentialaction.lua
local Action = require("gui.action")

local SequentialAction = {}
SequentialAction.__index = SequentialAction

setmetatable(SequentialAction, {
    __index = Action,
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

local function nop()
end

function SequentialAction:_init(...)
    Action._init(self, nop)

    self.actions = table.pack(...)

    self.current = 1
end

function SequentialAction:update()
    local action = self.actions[self.current]

    action:update()

    -- Go to next action in sequence
    if action.done then
        self.current = self.current + 1
        self.done = self.current > #self.actions
    end

end

return SequentialAction
