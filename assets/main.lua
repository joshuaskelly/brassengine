local draw = require("draw")

-- Called once at startup
function _init()
end

function _event(e)
    if e.type == "keydown" then
        print ("keydown: "..string.char(e.symbol))
    end


    return true
end

-- Called once per frame
function _update()
end

-- Called once per frame
function _draw()
    draw.clear(0)
    draw.text("hello world", 0, 0)
end
