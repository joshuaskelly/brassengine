local json = require("json")

local assets = require("assets")
local draw = require("draw")
local graphics = require("graphics")
local mouse = require("input.mouse")
local chevyfont = require("chevyfont")

-- Called once at startup
function _init()
    rt = graphics.get_render_texture()

    local f = io.open("data.json")
    game_data = json.decode(f:read("*all"))
    io.close(f)

    --font = chevyfont.new("fonts/pinch/atlas8.gif", "fonts/pinch/metrics.json")
    font = chevyfont.new("fonts/skullboy_mono/atlas8.gif", "fonts/skullboy_mono/metrics.json")
    font.size = 12
    --font = chevyfont.new("fonts/thicket_mono/atlas8.gif", "fonts/thicket_mono/metrics.json")

    scene = assets.get_texture(game_data.rooms[1].background)
    skull = assets.get_texture(game_data.objects[1].texture)
    frame = assets.get_texture("frame.gif")
    cursor = assets.get_texture("cursor.gif")
    graphics.set_transparent_color(0)

    sx, sy = 0, 0

    current_room = 1
end

-- Called once per frame
function _update()
end

-- Called once per frame
function _draw()
    draw.clear(0)

    -- Draw scene
    draw_room(current_room)

    -- Draw UI
    graphics.blit(frame, 0, 0, 320, 200, 0, 0, 320, 200)

    -- Draw text
    font:draw("The last thing that you", rt, 24, 144 + 0 * font.size)
    font:draw("remember is standing", rt, 24, 144 + 1 * font.size)
    font:draw("before the wizard Lakmir", rt, 24, 144 + 2 * font.size)

    font:draw("INVENTORY", rt, 216, 24)

    graphics.set_palette_color(1,2)
    font:draw("ENTRANCE", rt, 72, 8)
    graphics.set_palette_color(1,1)

    -- Draw cursor
    local x, y = mouse.position()
    graphics.blit(cursor, 0, 0, 16, 16, x - 4, y, 16, 16)

    --show_fps()
end

function draw_room(room_id)
    local x, y = mouse.position()

    graphics.set_clipping_rectangle(16, 16, 176, 112)

    if x > 16 and x < 192 and y > 16 and y < 128 then
        sx = math.floor((104 - x) / 88 * 16)
        sy = -math.floor(math.min(0, (72 - y) / 56 * -16))
    end

    local room = game_data.rooms[room_id]
    local background = assets.get_texture(room.background)

    graphics.blit(background, 0, 0, 208, 128, sx, sy, 208, 128)

    for i, o in ipairs(room.objects) do
        local object = game_data.objects[o.id]
        local sprite = assets.get_texture(object.texture)
        local x, y = o.position.x, o.position.y
        graphics.blit(sprite, 0, 0, 19, 22, x + sx, y + sy, 19, 22)
    end

    --graphics.blit(skull, 0, 0, 19, 22, 94 + sx, 34 + sy, 19, 22)

    graphics.set_clipping_rectangle(0, 0, 320, 200)
end

function show_fps()
    local dt = delta_time() / 1000
    local fps = 1 / dt
    local str = string.format("%.0f", fps)

    graphics.set_transparent_color(-1)
    draw.text(str, 8, 8)
    graphics.set_transparent_color(0)
end