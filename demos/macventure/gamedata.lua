--gamedata.lua
local json = require("json")

local f = io.open("data.json")
local game_data = json.decode(f:read("*all"))
io.close(f)

return game_data