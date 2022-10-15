local assets = require("assets")
local game_data = require("gamedata")
local graphics = require("graphics")
local mouse = require("input.mouse")

local GUI = require("gui")
local Image = require("gui.image")

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

    self:set_room(room_id)
    self.sx = 0
    self.sy = 0
end

function View:click(x, y)
    GUI.click(self, x - self.sx, y - self.sy)
end

function View:on_click(x, y)
    return true
end

function View:set_room(room_id)
    local room = game_data.rooms[room_id]

    self.room_id = room_id
    self.texture = assets.get_texture(room.background)
    self.children = {}
    local c = self.children

    for _, o in ipairs(room.objects) do
        local object = game_data.objects[o.id]
        local x, y = o.position.x, o.position.y

        local g = Image(object.texture, x, y)

        function g:on_click(x, y)
            self.visible = false
            return true
        end

        self:add_child(g)
    end
end

function View:update()
    GUI.update(self)

    local x, y = mouse.position()

    if x > self.rect.x and x < self.rect.x + self.rect.width and y > self.rect.y and y < self.rect.y + self.rect.height then
        self.sx = math.floor((104 - x) / 88 * 16)
        self.sy = -math.floor(math.min(0, (72 - y) / 56 * -16))
    end
end

function View:draw(offset_x, offset_y)
    graphics.set_clipping_rectangle(self.rect.x, self.rect.y, self.rect.width, self.rect.height)

    graphics.blit(self.texture, offset_x + self.sx, offset_y + self.sy)

    GUI.draw(self, offset_x + self.sx, offset_y + self.sy)

    graphics.set_clipping_rectangle(0, 0, 320, 200)
end

return View
