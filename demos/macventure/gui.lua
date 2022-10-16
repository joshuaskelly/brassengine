--gui.lua
local draw = require("draw")
local Rect = require("rect")

local GUI = {}
GUI.__index = GUI

setmetatable(GUI, {
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

function GUI:_init(x, y, width, height)
    self.children = {}
    self.rect = Rect.new(x, y, width, height)
    self.visible = true
end

function GUI:update()
    for _, child in ipairs(self.children) do
        if child.visible then
            child:update()
        end
    end
end

--t = 0
function GUI:draw(offset_x, offset_y)
    for _, child in ipairs(self.children) do
        if child.visible then
            child:draw(offset_x + self.rect.x, offset_y + self.rect.y)
        end
    end

    --draw.rectangle(offset_x + self.rect.x, offset_y + self.rect.y, self.rect.width, self.rect.height, math.floor(t) + 1)
    --t = (t + 0.05) % 2
end

function GUI:click(x, y)
    for i = #self.children, 1, -1 do
        local child = self.children[i]

        -- TODO: Why is the child nil sometimes?
        if child ~= nil and child.visible then
            if child:contains(x - self.rect.x, y - self.rect.y) then
                if child:click(x - self.rect.x, y - self.rect.y) then
                    return true
                end
            end
        end
    end

    return self:on_click(x, y)
end

function GUI:on_click(x, y)
    return false
end

function GUI.add_child(self, child)
    table.insert(self.children, child)
end

function GUI:remove(child)
    table.remove(self.children, child)
end

function GUI:contains(x, y)
    return self.rect:contains(x, y)
end

return GUI
