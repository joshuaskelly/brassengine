local assets = require("assets")
local display = require("display")
local draw = require("draw")
local graphics = require("graphics")
local mesh = require("mesh")
local statistics = require("statistics")

-- Called once at startup
function _init()
    local scale = 8
    display.set_size(128 * scale, 128 * scale)

    m = mesh.new_mesh()
    m.vertices = {20, 20, 108, 20, 20, 108, 108, 108}
    m.uvs = {0, 0, 1, 0, 0, 1, 1, 1}
    m.indices = {0, 1, 2, 3, 2, 1}

    t = assets.get_texture("texture.gif")
end

-- Called once per frame
function _update()
    local t =  0.01
    for i = 1, #m.vertices, 2 do
        local cs = math.cos(t)
        local sn = math.sin(t)
        local x = m.vertices[i] - 64
        local y = m.vertices[i + 1] - 64
        m.vertices[i] = (x * cs - y * sn) + 64
        m.vertices[i+1] = (x * sn + y * cs) + 64
    end
end

function stat()
    return (statistics.update_time() + statistics.draw_time()) / (1000 / 60) * 100
end

function draw_fps()
    graphics.set_palette_color(1, 7)
    draw.text(string.format("fps: %.0f", statistics.fps()), 0, 0)
    draw.text(string.format("cpu: %04.1f%%", stat()), 0, 8)
    graphics.set_palette_color(1, 1)
end

-- Called once per frame
function _draw()
    draw.clear(0)

    mesh.render(m, t)

    draw_fps()
end
