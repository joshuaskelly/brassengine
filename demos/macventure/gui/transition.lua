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
    self.times_looped = 0
    self.halfway_callback = nil
    self.end_callback = nil
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
    self.times_looped = 0
    self.halfway_callback = halfway_callback or nop
    self.end_callback = end_callback or nop
end

local function reset_draw_palette()
    for i = 0, 255 do
        engine.graphics.set_palette_color(i, i)
    end
end

local fore = 0
local back = 2
local half_effect_length = 500

function Transition:draw(offset_x, offset_y)
    for i = 0, 255 do
        -- Threshold palette
        local c = fore
        if ((time() - self.start_time) % half_effect_length) / half_effect_length * 128 >= i then
            c = back
        end

        if time() - self.start_time > half_effect_length then
            -- End
            if self.times_looped >= 1 then
                self.visible = false
                fore, back = 0, 2

                self.end_callback()

                return
            end

            -- Halfway
            self.halfway_callback()

            fore, back = back, fore
            self.start_time = time()
            self.times_looped = self.times_looped + 1
        end

        engine.graphics.set_palette_color(i, c)
    end

    Image.draw(self, offset_x, offset_y)
    reset_draw_palette()
end

function Transition:contains(x, y)
    return self.visible
end

function Image:on_click(x, y, button)
    return self.visible
end

return Transition
