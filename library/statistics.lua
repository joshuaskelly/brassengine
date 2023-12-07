--- @meta

--- Module for getting information about the runtime environment.
local statistics = {}

--- Gets time used by _draw function call in milliseconds
--- @return integer Time in milliseconds
function statistics.draw_time() end

--- Gets time used by _update function call in milliseconds
--- @return integer Time in milliseconds
function statistics.update_time() end

--- Gets current fps
--- @return number Frames per second
function statistics.fps() end

return statistics