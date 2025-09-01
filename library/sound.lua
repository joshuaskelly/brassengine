--- @meta

--- Module for playing sounds.
local sound = {}

--- Create new sound.
--- @param frame_count integer  Total number of PCM frames.
--- @return sound
function sound.new(frame_count) end

--- @class sound
--- @field pcm integer[]
--- @field frame_count integer
sound.sound = {}

--- Plays sound.
--- @param channel integer  Channel to play sound on. (optional)
function sound.sound:play(channel) end

--- Stops sound.
--- @param channel integer  Channel to stop playing. If omitted all sounds will be stopped. (optional)
function sound.sound:stop(channel) end

--- Sets volume.
--- @param channel integer  Channel to set volume for.
--- @param volume number  Volume level to set. Range 0.0 to 1.0
function sound.sound:volume(channel, volume) end

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

return sound