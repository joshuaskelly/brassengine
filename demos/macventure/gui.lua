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
    self.actions = {}
    self.children = {}
    self.rect = Rect.new(x, y, width, height)
    self.visible = true
end

function GUI:update()
    -- Update children
    for _, child in ipairs(self.children) do
        if child.visible then
            child:update()
        end
    end

    -- Run actions
    for _, action in ipairs(self.actions) do
        action:update()
    end

    for i = #self.actions, 1, -1 do
        local action = self.actions[i]

        if action.done then
            table.remove(self.actions, i)
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

function GUI:click(x, y, button)
    for i = #self.children, 1, -1 do
        local child = self.children[i]

        -- TODO: Why is the child nil sometimes?
        if child ~= nil and child.visible then
            if child:contains(x - self.rect.x, y - self.rect.y) then
                if child:click(x - self.rect.x, y - self.rect.y, button) then
                    return true
                end
            end
        end
    end

    return self:on_click(x, y, button)
end

function GUI:on_click(x, y, button)
    return false
end

local dragged = nil

function GUI:drag_start(x, y, button)
    dragged = nil

    for i = #self.children, 1, -1 do
        local child = self.children[i]

        -- TODO: Why is the child nil sometimes?
        if child ~= nil and child.visible then
            if child:contains(x - self.rect.x, y - self.rect.y) then
                if child:drag_start(x - self.rect.x, y - self.rect.y, button) then
                    if dragged == nil then
                        dragged = child
                    end

                    return true
                end
            end
        end
    end

    return self:on_drag_start(x, y, button)
end

function GUI:on_drag_start(x, y, button)
    return false
end

function GUI:drag(x, y, button)
    if dragged then
        dragged:on_drag(x, y, button)
    end
end

function GUI:on_drag(x, y, button)
end

function GUI:drag_end(x, y, button)
    if dragged then
        dragged:on_drag_end(x, y, button)
    end
end

function GUI:on_drag_end(x, y, button)
end

function GUI.add_child(self, child)
    table.insert(self.children, child)
end

function GUI:remove(child)
    local index = 0

    for i, c in ipairs(self.children) do
        if c == child then
            index = i
            break
        end
    end

    table.remove(self.children, index)
end

function GUI:contains(x, y)
    return self.rect:contains(x, y)
end

function GUI:add_action(action)
    if action.target == nil then
        action:set_target(self)
        table.insert(self.actions, action)
    end
end

function GUI:remove_action(action)
    local index = 0

    for i, a in ipairs(self.actions) do
        if a == action then
            index = i
            break
        end
    end

    table.remove(self.actions, index)
end

return GUI
