local engine = {}

engine.assets = require("assets")
engine.draw = require("draw")
engine.graphics = require("graphics")
engine.graphics.texture = require("graphics.texture")
engine.input = {}
engine.input.keyboard = require("input.keyboard")
engine.input.mouse = require("input.mouse")

return engine