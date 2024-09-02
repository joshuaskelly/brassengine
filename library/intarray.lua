--- @meta

--- Module for working with native C arrays of integer data.
local intarray = {}

--- @class intarray
intarray.intarray = {}

--- Returns a new intarray
--- @param size integer  Number of elements.
--- @return intarray 
function intarray.new(size) end

--- Adds given value to end of int array.
--- @param array intarray  Array to modify.
--- @param value integer  Value to add
function intarray.add(array, value) end

--- Resize intarray to new length.
--- @param array intarray  int array to modify.
--- @param size integer  New total number of elements
function intarray.resize(array, size) end

return intarray