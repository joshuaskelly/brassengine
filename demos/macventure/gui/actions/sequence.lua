--sequence.lua
local Action = require("gui.action")

local Sequence = {}
Sequence.__index = Sequence

setmetatable(Sequence, {
    __index = Action,
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

local function nop()
end

function Sequence:_init(...)
    Action._init(self, nop)

    self.actions = table.pack(...)

    self.current = 1
end

function Sequence:update()
    local action = self.actions[self.current]

    action:update()

    -- Go to next action in sequence
    if action.done then
        self.current = self.current + 1
        self.done = self.current > #self.actions
    end

end

function Sequence:set_target(target)
    Action.set_target(self, target)

    for _, action in ipairs(self.actions) do
        action:set_target(target)
    end
end

return Sequence
