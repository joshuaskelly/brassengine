local engine = require("engine")

local Image = require("gui.image")

local Transition = {}
Transition.__index = Transition

setmetatable(Transition, {
    __index = Image,
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

function Transition:_init(texture_name, x, y)
    Image._init(self, texture_name, x, y)

    self.start_time = 0
    self.halfway_callback = nil
    self.end_callback = nil
    self.state = nil
end

local direction_textures = {
    forward = engine.assets.get_texture("textures/vfx/transition_forward.gif"),
    backward = engine.assets.get_texture("textures/vfx/transition_backward.gif"),
    left = engine.assets.get_texture("textures/vfx/transition_left.gif"),
    right = engine.assets.get_texture("textures/vfx/transition_right.gif"),
    up = engine.assets.get_texture("textures/vfx/transition_up.gif"),
    down = engine.assets.get_texture("textures/vfx/transition_down.gif")
}

local function nop()
end

function Transition:play(direction, halfway_callback, end_callback)
    self.texture = direction_textures[direction] or direction_textures["forward"]

    self.visible = true
    self.start_time = time()
    self.halfway_callback = halfway_callback or nop
    self.end_callback = end_callback or nop
    self.state = Transition.animate_in
end

local function reset_draw_palette()
    for i = 0, 255 do
        engine.graphics.set_palette_color(i, i)
    end
end

local animate_in_time = 350
local animate_hold_time = 300
local animate_out_time = 350

function Transition:draw(offset_x, offset_y)
    self:state()

    Image.draw(self, offset_x, offset_y)
    reset_draw_palette()
end

function Transition:animate_in()
    for i = 0, 255 do
        -- Threshold palette
        local c = 0
        if (time() - self.start_time) / animate_in_time * 128 >= i then
            c = 2
        end

        engine.graphics.set_palette_color(i, c)
    end

    if time() - self.start_time >= animate_in_time then
        self.halfway_callback()
        self.state = Transition.animate_hold
        self.start_time = time()
    end
end

function Transition:animate_hold()
    for i = 0, 255 do
        engine.graphics.set_palette_color(i, 2)
    end

    if time() - self.start_time >= animate_hold_time then
        self.state = Transition.animate_out
        self.start_time = time()
    end
end

function Transition:animate_out()
    for i = 0, 255 do
        -- Threshold palette
        local c = 2
        if (time() - self.start_time) / animate_out_time * 128 >= i then
            c = 0
        end

        engine.graphics.set_palette_color(i, c)
    end

    if time() - self.start_time >= animate_out_time then
        self.visible = false
        self.end_callback()
    end
end

function Transition:contains(x, y)
    return self.visible
end

function Image:on_click(x, y, button)
    return self.visible
end

return Transition
