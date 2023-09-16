local ray = require("ray")

function _init()
    renderer = ray.Renderer:new()
end

function _update()
end

function _draw()
    renderer:clear("all")
end
