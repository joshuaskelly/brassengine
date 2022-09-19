-- Testing blitting stuff
local draw = require("draw")

-- Called once at startup
function _init()
    print("lua init")
end

-- Called once per frame
function _update()
end

-- Called once per frame
function _draw()
    draw.clear(0)
    local x, y = mouse_position()

    frame("ENTRANCE")
    grid()
end

function sprite(index, x, y)
    sx = (index % 32) * 8
    sy = (index // 32) * 8
    draw.test_blit(sx, sy, 8, 8, x, y, 8, 8)
end

function frame(name)
    -- Draw name
    draw.text(name, 16, 8)

    local s = 398

    -- Draw top + bottom frame
    for x = 2, (320 - 24) // 8 do
        if x > #name + 2 then
            sprite(s, x * 8, 8)
        end

        sprite(s, x * 8, 200 - 16)
    end

    -- Draw left + right frame
    for y = 2, (200 - 24) // 8 do
        if y > 1 then
            sprite(s + 32, 8, y * 8)
        end

        sprite(s + 32, 320 - 16, y * 8)
    end

    -- Draw corners
    sprite(s + 62, 8, 8)
    sprite(s + 63, 320 - 16, 8)
    sprite(s + 63 + 31, 8, 200 - 16)
    sprite(s + 63 + 32, 320 - 16, 200 - 16)
end

function grid()
    for y = 2, (200 // 8) - 3 do
        for x = 2, (320 // 8) - 3 do
            draw.rectangle(x * 8, y * 8, 8, 8, 4)
        end
    end
end
