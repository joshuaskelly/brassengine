local assets = require("assets")
local game_data = require("gamedata")
local graphics = require("graphics")
local mouse = require("input.mouse")

local GUI = require("gui")
local Image = require("gui.image")
local Rect = require("rect")
local scripting = require("scripting")

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
    GUI._init(self, 16, 16, 176, 112)

    self.original_rect = Rect.new(16, 16, 176, 112)

    self:set_room(room_id)
end

function View:on_click(x, y, button)
    return true
end

function View:set_room(room_id)
    local room = game_data.rooms[room_id]

    self.room_id = room_id
    self.texture = assets.get_texture(room.background)
    self.children = {}

    for _, o in ipairs(room.objects) do
        local object = game_data.objects[o.id]
        local x, y = o.position.x, o.position.y

        local g = Image(object.texture, x, y)

        function g:on_click(x, y, button)
            if button == 1 then
                scripting.execute(object.interact)
            elseif button == 3 then
                scripting.execute(object.inspect)
            end
            --self.visible = false
            return true
        end

        self:add_child(g)
    end
end

function View:update()
    GUI.update(self)

    local x, y = mouse.position()
    local sx, sy = 0, 0

    if self.original_rect:contains(x, y) then
        sx = math.floor((104 - x) / 88 * 16)
        sy = -math.floor(math.min(0, (72 - y) / 56 * -16))
    end

    self.rect.x = sx
    self.rect.y = sy
end

function View:draw(offset_x, offset_y)
    graphics.set_clipping_rectangle(self.original_rect.x, self.original_rect.y, self.original_rect.width, self.original_rect.height)

    graphics.blit(self.texture, offset_x + self.rect.x, offset_y + self.rect.y)
    GUI.draw(self, offset_x, offset_y)

    graphics.set_clipping_rectangle(0, 0, 320, 200)
end

return View
