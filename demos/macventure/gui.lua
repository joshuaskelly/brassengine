--gui.lua
local assets = require("assets")
local Rect = require("rect")
local game_data = require("gamedata")
local graphics = require("graphics")
local mouse = require("input.mouse")
local draw = require("draw")
local texture = require("graphics.texture")

local module = {}
module.__index = module

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
end

function GUI:update()
    for _, child in ipairs(self.children) do
        child:update()
    end
end

--t = 0
function GUI:draw(offset_x, offset_y)
    for _, child in ipairs(self.children) do
        child:draw(offset_x, offset_y)
    end

    --draw.rectangle(offset_x + self.rect.x, offset_y + self.rect.y, self.rect.width, self.rect.height, math.floor(t) + 1)
    --t = (t + 0.05) % 2
end

function GUI:click(x, y)
    for i = #self.children, 1, -1 do
        local child = self.children[i]

        if child:contains(x, y) then
            if child:click(x, y) then
                return true
            end
        end
    end

    return self:on_click(x, y)
end

function GUI:on_click(x, y)
    return false
end

function GUI:add_child(child)
    table.insert(self.children, child)
end

function GUI:contains(x, y)
    return self.rect:contains(x, y)
end

local Image = {}
Image.__index = Image

setmetatable(Image, {
    __index = GUI,
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

function Image:_init(texture_name, x, y)
    local texture = assets.get_texture(texture_name)
    local width, height = texture:size()

    GUI._init(self, x, y, width, height)

    self.texture = texture
end

function Image:draw(offset_x, offset_y)
    graphics.blit(self.texture, offset_x + self.rect.x, offset_y + self.rect.y)
    GUI.draw(self, offset_x, offset_y)
end

function Image:on_click(x, y)
    print("Clicked!")
    return true
end

function Image:contains(x, y)
    local color = texture.get_pixel(self.texture, x - self.rect.x, y - self.rect.y)
    return color ~= 0
end

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
    GUI.click(self, x - sx, y - sy)
end

function View:on_click(x, y)
    return true
end

function View:set_room(room_id)
    local room = game_data.rooms[room_id]

    self.room_id = room_id
    self.texture = assets.get_texture(room.background)
    self.children = {}

    for i, o in ipairs(room.objects) do
        local object = game_data.objects[o.id]
        local x, y = o.position.x, o.position.y

        local g = Image(object.texture, x, y)

        function g:update()
        end

        self:add_child(g)
    end
end

function View:update()
    GUI.update(self)

    local x, y = mouse.position()

    if x > self.rect.x and x < self.rect.x + self.rect.width and y > self.rect.y and y < self.rect.y + self.rect.height then
        sx = math.floor((104 - x) / 88 * 16)
        sy = -math.floor(math.min(0, (72 - y) / 56 * -16))
    end
end

function View:draw(offset_x, offset_y)
    graphics.set_clipping_rectangle(self.rect.x, self.rect.y, self.rect.width, self.rect.height)

    graphics.blit(self.texture, offset_x + sx, offset_y + sy)

    GUI.draw(self, offset_x + sx, offset_y + sy)

    graphics.set_clipping_rectangle(0, 0, 320, 200)
end

module.GUI = GUI
module.Image = Image
module.View = View

return module