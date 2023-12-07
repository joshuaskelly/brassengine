--- @meta

--- Global functions.
local globals = {}

--- Prints given object to console.
--- @param arg any  Object to print to console
function globals.print(arg) end

--- Gets time since last frame.
--- @return integer Time elapsed in milliseconds.
function globals.delta_time() end

--- Gets time since app launch.
--- @return integer Time elapsed in milliseconds.
function globals.time() end

--- Quits app.
function globals.quit() end

return globals