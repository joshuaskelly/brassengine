local gamedata = require("gamedata")
local GUI = require("gui")
local Image = require("gui.image")

local MiniMap = {}
MiniMap.__index = MiniMap

setmetatable(MiniMap, {
    __index = GUI,
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

function MiniMap:_init(x, y)
    GUI._init(self, x, y, 40, 40)
end

function MiniMap:set_room(room_id)
    self.children = {}

    local room = gamedata.rooms[room_id]

    for _, exit in ipairs(room.exits) do
        local x = (exit.slot % 5) * 8
        local y = (exit.slot // 5) * 8

        local e = Image("textures/ui/exit.gif", x, y)
        function e:on_click(x, y)
            game_screen:set_room(exit.room_id)
        end

        self:add_child(e)
    end
end

return MiniMap
