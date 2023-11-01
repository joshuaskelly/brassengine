local assets = require("assets")
local draw = require("draw")
local keyboard = require("keyboard")
local sound = require("sound")

-- Called once at startup
function _init()
    s = assets.get_sound("sound.wav")
end

local key_down = false

-- Called once per frame
function _update()
    if keyboard.key(44) then
        if not key_down then
            sound.play(s)
            key_down = true
        end
    else
        key_down = false
    end
end

-- Called once per frame
function _draw()
    draw.clear(0)
    draw.text("hello world", 0, 0)
end
