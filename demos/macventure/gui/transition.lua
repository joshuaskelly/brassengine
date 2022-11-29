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

    self.effect_time = 0
    self.times_looped = 0
    self.halfway_callback = nil
    self.end_callback = nil
end

function Transition:play(halfway_callback, end_callback)
    self.visible = true
    self.effect_time = time()
    self.times_looped = 0
    self.halfway_callback = halfway_callback
    self.end_callback = end_callback
end

local function reset_draw_palette()
    for i = 0, 255 do
        engine.graphics.set_palette_color(i, i)
    end
end

local fore = 0
local back = 2

function Transition:draw(offset_x, offset_y)
    for i = 0, 255 do
        local c = fore
        if ((time() - self.effect_time) % 750) * 176 * 2 // 1000 >= i then
            c = back
        end

        if time() - self.effect_time > 750 then
            if self.times_looped >= 1 then
                self.visible = false
                fore, back = 0, 2

                if self.end_callback then
                    self.end_callback()
                end


                return
            end

            if self.halfway_callback then
                self.halfway_callback()
            end

            fore, back = back, fore
            self.effect_time = time()
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
