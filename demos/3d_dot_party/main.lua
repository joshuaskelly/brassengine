-- port of @lexaloffle's
-- 3d dot party tutorial
-- https://twitter.com/lexaloffle/status/1004070743284408320
require("pico")

frame = 0

function t()
    frame = frame + 0.0001
    return frame
end

function _init()
    -- make some points
    -- 7x7x7 cube

    pt={}
    for z = -1, 1, 1 / 3 do
        for y = -1, 1, 1 / 3 do
            for x = -1, 1, 1 / 3 do
                p = {}
                p.x, p.y, p.z = x, y, -z
                p.col = 8 + (x * 2 + y * 3) % 8
                add(pt, p)
            end
        end
    end
end

--rotate x,y by angle a
function rot(x, y, a)
    local x0 = x
    x = math.cos(a) * x - math.sin(a) * y
    y = math.cos(a) * y + math.sin(a) * x0

    return x, y
end

function _draw()
    cls()
    for p in all(pt) do
        -->camera space
        p.cx, p.cz = rot(p.x, p.z, t() / 8)
        p.cy, p.cz = rot(p.y, p.cz, t() / 7)

        p.cz = p.cz + 2 + math.cos(t()/6)

    end

    --sort by distance from camera
    --because they go out of order
    --slowly, doing a bubble sort
    --up and down 3 times is good
    --enough

    for pass = 1, 3 do
        for i = 1, #pt - 1 do
            if pt[i].cz < pt[i + 1].cz then
                pt[i],pt[i + 1] = pt[i + 1], pt[i]
            end
        end
        for i = #pt - 1, 1, -1 do
            if pt[i].cz < pt[i + 1].cz then
                pt[i], pt[i + 1] = pt[i + 1], pt[i]
            end
        end
    end

    rad1 = 5 + math.cos(t() / 4) * 4

    for p in all(pt) do
        --> screen space
        sx = 64 + p.cx * 64 / p.cz
        sy = 64 + p.cy * 64 / p.cz
        rad = rad1 / p.cz

        if (p.cz > 0.1) then -- clip
            circfill(sx, sy, rad, p.col)
            circfill(sx + rad / 3, sy - rad / 3,
            rad / 3, 7)
        end
    end
end
