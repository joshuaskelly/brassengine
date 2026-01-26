--- @meta

--- Module that extends the standard Lua math library with useful functions for game development.
local math = {}

--- Clamps given value to specified minimum and maximum range.
--- @param value number
--- @param min number
--- @param max number
--- @return number Clamped value between min and max.
function math.clamp(value, min, max) end

--- Linearly interpolate between a and b by t.
--- @param a number  Start value
--- @param b number  End value
--- @param t number  Interpolation value
--- @return number Interpolated value between a and b.
function math.lerp(a, b, t) end

--- Linearly interpolate between a and b by t.
--- @param a number  Start value
--- @param b number  End value
--- @param v number  Value in range [a, b]
--- @return number Value where v is between a and b in range [0, 1].
function math.inverse_lerp(a, b, v) end

--- Linearly map given value from source range [a, b] to corresponding value in dest range [c, d].
--- @param number v  Value in start range
--- @param number v  Value in start range
--- @param number v  Value in start range
--- @param number v  Value in start range
--- @param number v  Value in start range
--- @return number Corresponding value in range [c, d]
function math.remap(number, number, number, number, number) end

--- Returns a Perlin noise value.
--- @param x number  X-coordinate of sample point.
--- @param y number  Y-coordinate of sample point. (optional)
--- @param z number  Z-coordinate of sample point. (optional)
--- @return number float Sampled point.
function math.noise(x, y, z) end

--- Interpolates between a and b with smoothing at the limits.
--- @param a number  Start value
--- @param b number  End value
--- @param t number  Interpolation value
--- @return number float
function math.smoothstep(a, b, t) end

--- Return sign of given number.
--- @param f number
--- @return  number float
function math.sign(f) end

return math