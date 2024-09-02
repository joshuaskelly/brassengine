--- @meta

--- Module for working with native C arrays of floating point data.
local floatarray = {}

--- @class floatarray
floatarray.floatarray = {}

--- Returns a new floatarray
--- @param size integer  Number of elements.
--- @return floatarray 
function floatarray.new(size) end

--- Adds given value to end of float array.
--- @param array floatarray  Array to modify.
--- @param value number  Value to add
function floatarray.add(array, value) end

--- Resize floatarray to new length.
--- @param array floatarray  Float array to modify.
--- @param size integer  New total number of elements
function floatarray.resize(array, size) end

return floatarray