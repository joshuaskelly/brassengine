--- @meta

--- Module for working with vector4 data.
local vector4 = {}

--- @class vector4
--- @field x number
--- @field y number
--- @field z number
--- @field w number
vector4.vector4 = {}

--- Returns a new vector4
--- @param x number? 
--- @param y number? 
--- @param z number? 
--- @param w number? 
--- @return vector4 
function vector4.new(x, y, z, w) end

--- Returns a vector made from the sign of it's components.
--- @param v0 vector4 
--- @return vector4 
function vector4.sign(v0) end

--- Returns a vector made from snapping the components to given resolution.
--- @param v0 vector4 
--- @param f number  Resolution of snap
--- @return vector4 
function vector4.snap(v0, f) end

--- Negates a vector.
--- @param v0 vector4 
--- @return vector4 
function vector4.negative(v0) end

--- Returns a vector made from the absolute values of the components.
--- @param v0 vector4 
--- @return vector4 
function vector4.abs(v0) end

--- Returns a vector made from the floor of the components.
--- @param v0 vector4 
--- @return vector4 
function vector4.floor(v0) end

--- Returns a vector made from the ceil of the components.
--- @param v0 vector4 
--- @return vector4 
function vector4.ceil(v0) end

--- Returns a vector made from rounding the components.
--- @param v0 vector4 
--- @return vector4 
function vector4.round(v0) end

--- Returns a vector that is the component-wise max of v0 and v1.
--- @param v0 vector4 
--- @param v1 vector4 
--- @return vector4 
function vector4.max(v0, v1) end

--- Returns a vector that is the component-wise min of v0 and v1.
--- @param v0 vector4 
--- @param v1 vector4 
--- @return vector4 
function vector4.min(v0, v1) end

--- Returns a vector that is a component-wise clamp of v0 such that min < v0 < max.
--- @param v0 vector4  Vector to clamp
--- @param min vector4  Min vector
--- @param max vector4  Max vector
--- @return vector4 
function vector4.clamp(v0, min, max) end

--- Returns a vector in the direction of v0 with magnitude 1.
--- @param v0 vector4 
--- @return vector4 
function vector4.normalize(v0) end

--- Linearly interpolate between v0 and v1.
--- @param v0 vector4 
--- @param v1 vector4 
--- @param t number  Value used to interpolate between v0 and v1.
--- @return vector4 
function vector4.lerp(v0, v1, t) end

return vector4