--- @meta

--- Module for playing sounds.
local sound = {}

--- @class sound
--- @field pcm integer[]
--- @field frame_count integer
sound.sound = {}

--- Plays sound.
--- @param channel integer  Channel to play sound on. (optional)
function sound.sound:play(channel) end

--- Returns a copy of this sound.
--- @return sound 
function sound.sound:copy() end

--- Sets PCM data for given frame.
--- @param index integer  Frame index
--- @param value integer  PCM data value
function sound.sound:set_frame(index, value) end

--- Gets PCM data for given frame.
--- @param index integer  Frame index
--- @return integer PCM data
function sound.sound:get_frame(index) end

--- Create new sound.
--- @param frame_count integer  Total number of PCM frames.
--- @return sound 
function sound.new(frame_count) end

return sound