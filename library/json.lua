--- @meta

--- Module for working with JSON.
local json = {}

--- Returns a string representing given object encoded in JSON.
--- @param object table  Lua table to encode
--- @return string JSON string representation.
function json.encode(object) end

--- Returns an object representing given JSON string.
--- @param string string  String to decode
--- @return table Table from string representation.
function json.decode(string) end

return json