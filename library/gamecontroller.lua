--- @meta

--- Module for getting gamecontroller input.
local gamecontroller = {}

--- Check if given button is down.
--- @param id integer Controller id.
--- @param button integer Button to check.
--- @return boolean True if button is down.
function gamecontroller.button(id, button) end

--- Get current value of given axis.
--- @param id integer Controller id.
--- @param axis integer  Axis to check.
--- @return number
function gamecontroller.axis(id, axis) end

--- Get number of connected controllers.
--- @return integer
function gamecontroller.count() end

--- Get an array of connected controller ids.
--- @return integer[]
function gamecontroller.connected() end

--- Check if given controller id is connected.
--- @tparam id integer Controller id.
--- @return boolean true if given id is connected, false otherwise.
function gamecontroller.connected(id) end

--- Axis enums
gamecontroller.axes = {
    LEFTX = 0,
    LEFTY = 1,
    RIGHTX = 2,
    RIGHTY = 3,
    TRIGGERLEFT = 4,
    TRIGGERRIGHT = 5,
}

--- Button enums
gamecontroller.buttons = {
    A = 0,
    B = 1,
    X = 2,
    Y = 3,
    BACK = 4,
    GUIDE = 5,
    START = 6,
    LEFTSTICK = 7,
    RIGHTSTICK = 8,
    LEFTSHOULDER = 9,
    RIGHTSHOULDER = 10,
    DPAD_UP = 11,
    DPAD_DOWN = 12,
    DPAD_LEFT = 13,
    DPAD_RIGHT = 14,
}

return gamecontroller
