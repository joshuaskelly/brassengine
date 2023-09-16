---
-- @module gifrecorder

local graphics = require("graphics")
local gif = require("gif")
local keyboard = require("keyboard")
local texture = require("texture")

local gifrecorder = {}

local KEY_F7 = 64
local KEY_F8 = 65

gifrecorder.Recorder = {}

--- Create a new recorder object
function gifrecorder.Recorder:new()
    local recorder = {}
    setmetatable(recorder, self)
    self.__index = self

    recorder.frames = {}
    recorder.is_recording = false
    recorder.is_key_down = keyboard.key(KEY_F8)

    return recorder
end

--- Record a single frame
function gifrecorder.Recorder:record()
    local render_texture = graphics.get_render_texture()
    table.insert(self.frames, texture.copy(render_texture))
end

--- Save gif with given filename
---@param filename string
function gifrecorder.Recorder:save(filename)
    gif.save(filename, self.frames)
end

--- Clear internal frame buffer
function gifrecorder.Recorder:reset()
    self.frames = {}
end

--- Update recorder state. Should be called each frame.
function gifrecorder.Recorder:update()
    if not self.is_key_down then
        -- Take a single screenshot
        if keyboard.key(KEY_F7) then
            local name = string.format("screenshot_%s.gif", os.date("%Y%m%d%H%M%S"))

            print(string.format("Saving %s", name))

            gif.save(name, texture.copy(graphics.get_render_texture()))

        -- Record the screen
        elseif keyboard.key(KEY_F8) then
            -- If currently recording save and stop
            if self.is_recording then
                local name = string.format("screenshot_%s.gif", os.date("%Y%m%d%H%M%S"))

                print(string.format("Saving %s (%s frames)", name, #self.frames))

                self:save(name)
                self:reset()
            end

            self.is_recording = not self.is_recording
        end
    end

    self.is_key_down = keyboard.key(KEY_F7) or keyboard.key(KEY_F8)

    if self.is_recording then
        self:record()
    end
end

return gifrecorder
