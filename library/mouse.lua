--- @meta

--- Module for getting mouse input.
local mouse = {}

--- Check if given button is down.
--- @param button integer  Button index to check. Primary button is 1, middle button is 2, secondary button is 3.
--- @return boolean True if button is down, false otherwise
function mouse.button(button) end

--- Get current mouse position.
--- @return integer Mouse x coord
--- @return integer Mouse y coord
function mouse.position() end

--- Get mouse relative motion.
--- @return integer Mouse x coord
--- @return integer Mouse y coord
function mouse.motion() end

--- Get mouse wheel scroll values.
--- @return integer Horizontal scroll amount
--- @return integer Vertical scroll amount
function mouse.wheel() end

--- Set mouse grab state.
--- @param grabbed boolean  True if mouse should be constrained to window.
function mouse.set_grabbed(grabbed) end

return mouse