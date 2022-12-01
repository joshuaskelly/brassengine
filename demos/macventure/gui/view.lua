local assets = require("assets")
local game_data = require("gamedata")
local graphics = require("graphics")
local mouse = require("input.mouse")
local texture = require("graphics.texture")

local GUI = require("gui")
local Image = require("gui.image")
local Rect = require("rect")
local scripting = require("scripting")
local Delay = require("gui.actions.delay")
local MoveTo = require("gui.actions.moveto")
local Sequence = require("gui.actions.sequence")

local View = {}
View.__index = View

setmetatable(View, {
    __index = GUI,
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

function View:_init(room_id)
    GUI._init(self, 0, 0, 176, 112)

    self.original_rect = Rect.new(16, 16, 176, 112)

    self:set_room(room_id)
end

function View:contains(x, y)
    return self.original_rect:contains(x, y)
end

function View:on_click(x, y, button)
    x = x - self.rect.x
    y = y - self.rect.y

    local room = game_data.rooms[self.room_id]
    if room.pick_mask == nil then
        return
    end

    local pick_texture = assets.get_texture(room.pick_mask)
    if pick_texture == nil then
        return
    end

    local picked = texture.get_pixel(pick_texture, x, y)

    if button == 1 then
        local str = string.format(room.interact, picked)
        scripting.execute(str)
    elseif button == 3 then
        local str = string.format(room.inspect, picked)
        scripting.execute(str)
    end

    return true
end

function View:set_room(room_id)
    local room = game_data.rooms[room_id]

    self.room_id = room_id
    self.texture = assets.get_texture(room.background)
    self.children = {}
    self.actions = {}
    self.rect.x = 0
    self.rect.y = 0

    for _, o in ipairs(room.objects) do
        local object = game_data.objects[o.id]
        local x, y = o.position.x, o.position.y

        local g = Image(object.texture, x, y)
        g.id = o.id

        function g:on_click(x, y, button)
            if button == 1 then
                scripting.execute(object.interact)
            elseif button == 3 then
                scripting.execute(object.inspect)
            end
            --self.visible = false
            return true
        end

        function g:on_drag_start(x, y, button)
            -- TODO: drag start logic
            return true
        end

        function g:on_drag(x, y, button)
            -- TODO: drag logic
        end

        function g:on_drag_end(x, y, button)
            -- TODO: drop logic
        end

        self:add_child(g)
    end
end

local function lerp(a, b, t)
    return a * (1 - t) + b * t
end

local mouse_over = false

function View:update()
    GUI.update(self)

    local x, y = mouse.position()
    local sx, sy = 0, 0

    if self.original_rect:contains(x, y) then
        mouse_over = true

        -- Smooth blend to mouse position
        sx = math.floor((104 - x) / 88 * 16)
        sy = -math.floor(math.min(0, (72 - y) / 56 * -16))
        self.rect.x = math.floor(lerp(self.rect.x, sx, 0.35))
        self.rect.y = math.floor(lerp(self.rect.y, sy, 0.35))

        -- Clear out actions
        self.actions = {}
    elseif mouse_over then
        mouse_over = false

        self:add_action(
            Sequence(
                Delay(2000),
                MoveTo(0, 0, 1000)
            )
        )
    end
end

function View:draw(offset_x, offset_y)
    graphics.set_clipping_rectangle(self.original_rect.x, self.original_rect.y, self.original_rect.width, self.original_rect.height)

    graphics.blit(self.texture, offset_x + self.rect.x, offset_y + self.rect.y)
    GUI.draw(self, offset_x, offset_y)

    graphics.set_clipping_rectangle(0, 0, 320, 200)
end

return View
