local gamedata = require("gamedata")
local GUI = require("gui")
local DescriptionText = require("gui.description")
local Image = require("gui.image")
local View = require("gui.view")
local Text = require("gui.text")
local Transition = require("gui.transition")
local MiniMap = require("gui.minimap")
local scripting = require("scripting")

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

    -- Scene view
    self.view = View(room_id)
    self:add_child(self.view)

    -- Minimap
    self:add_child(Image("textures/gui/minimap.gif", 20, 140))
    self.minimap = MiniMap(20, 140)
    self:add_child(self.minimap)

    -- Transition
    self.transition = Transition("textures/vfx/transition_forward.gif", 16, 16)
    self.transition.visible = false
    self:add_child(self.transition)

    -- UI frame
    self:add_child(Image("textures/gui/frame.gif", 0, 0))

    -- Room name
    self.room_name = Text("", 72, 8)
    self:add_child(self.room_name)

    -- Description text
    self.description = DescriptionText(76, 140)
    self:add_child(self.description)

    -- Inventory
    self:add_child(Text("INVENTORY", 216, 24))
end

function GameScreen:move_to(room_id, direction)
    local room = gamedata.rooms[room_id]

    self.transition:play(
        direction,
        function()
            self:set_room(room_id)
        end,
        function()
            scripting.execute(room.enter)
        end
    )
end

function GameScreen:set_room(room_id)
    local room = gamedata.rooms[room_id]

    self.view:set_room(room_id)
    self.minimap:set_room(room_id)
    self.room_name:set(room.name)
end

function GameScreen:describe(text)
    self.description:set(text)
end

return GameScreen
