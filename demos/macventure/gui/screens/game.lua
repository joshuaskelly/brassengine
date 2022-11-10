local gamedata = require("gamedata")
local GUI = require("gui")
local DescriptionText = require("gui.description")
local Image = require("gui.image")
local View = require("gui.view")
local Text = require("gui.text")
local MiniMap = require("gui.minimap")

local GameScreen = {}
GameScreen.__index = GameScreen

setmetatable(GameScreen, {
    __index = GUI,
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

function GameScreen:_init(room_id)
    GUI._init(self, 0, 0, 320, 200)

    self.view = View(room_id)
    self:add_child(self.view)

    self:add_child(Image("textures/ui/frame.gif", 0, 0))

    self.minimap = MiniMap(20, 140)
    self:add_child(self.minimap)

    self.room_name = Text("", 72, 8)
    self:add_child(self.room_name)

    self:add_child(Text("INVENTORY", 216, 24))

    self:add_child(DescriptionText(76, 140))

    self:set_room(room_id)
end

function GameScreen:set_room(room_id)
    local room = gamedata.rooms[room_id]

    self.view:set_room(room_id)
    self.minimap:set_room(room_id)
    self.room_name:set(room.name)
end

return GameScreen
