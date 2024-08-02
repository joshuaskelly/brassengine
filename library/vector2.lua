--- @meta

--- Module for working with vector2 data.
local vector2 = {}

--- @class vector2
--- @field length number
--- @field x number
--- @field y number
vector2.vector2 = {}

--- Returns a new vector2
--- @param x number?
--- @param y number?
--- @return vector2
function vector2.new(x, y) end

--- Returns a vector made from snapping the components to given resolution.
--- @param v0 vector2
--- @param f number  Resolution of snap
--- @return vector2
function vector2.snap(v0, f) end

--- Negates a vector.
--- @param v0 vector2
--- @return vector2
function vector2.negative(v0) end

--- Returns a vector made from the absolute values of the components.
--- @param v0 vector2
--- @return vector2
function vector2.abs(v0) end

--- Returns a vector made from the floor of the components.
--- @param v0 vector2
--- @return vector2
function vector2.floor(v0) end

--- Returns a vector made from the ceil of the components.
--- @param v0 vector2
--- @return vector2
function vector2.ceil(v0) end

--- Returns a vector made from rounding the components.
--- @param v0 vector2
--- @return vector2
function vector2.round(v0) end

--- Returns a vector that is the component-wise max of v0 and v1.
--- @param v0 vector2
--- @param v1 vector2
--- @return vector2
function vector2.max(v0, v1) end

--- Returns a vector that is the component-wise min of v0 and v1.
--- @param v0 vector2
--- @param v1 vector2
--- @return vector2
function vector2.min(v0, v1) end

--- Returns a vector that is a component-wise clamp of v0 such that min < v0 < max.
--- @param v0 vector2  Vector to clamp
--- @param min vector2  Min vector
--- @param max vector2  Max vector
--- @return vector2
function vector2.clamp(v0, min, max) end

--- Returns a vector in the direction of v0 with magnitude 1.
--- @param v0 vector2
--- @return vector2
function vector2.normalize(v0) end

--- Dot product of two vectors.
--- @param v0 vector2
--- @param v1 vector2
--- @return number Dot product
function vector2.dot(v0, v1) end

--- Project v0 onto v1.
--- @param v0 vector2
--- @param v1 vector2
--- @return vector2
function vector2.project(v0, v1) end

--- Reflect v0 off of plane given by normal.
--- @param v0 vector2
--- @param normal vector2  Plane normal
--- @return vector2
function vector2.reflect(v0, normal) end

--- Returns a vector rotated 90 degrees clockwise from v0.
--- @param v0 vector2
--- @return vector2
function vector2.tangent(v0) end

--- Returns a vector that is rotated clockwise by angle in radians from v0.
--- @param v0 vector2
--- @param radians number  Angle to rotate
--- @return vector2
function vector2.rotate(v0, radians) end

--- Linearly interpolate between v0 and v1.
--- @param v0 vector2
--- @param v1 vector2
--- @param t number  Value used to interpolate between v0 and v1.
--- @return vector2
function vector2.lerp(v0, v1, t) end

--- Get counterclockwise angle in radians between the positive x-axis and v0.
--- @param v0 vector2
--- @return number Angle in radians.
function vector2.angle(v0) end

--- Get vector magnitude.
--- @param v0 vector2
--- @return number Length
function vector2.length(v0) end

--- Get vector squared magnitude.
--- @param v0 vector2
--- @return number Squared length
function vector2.length_squared(v0) end

--- Get distance between v0 and v1.
--- @param v0 vector2
--- @param v1 vector2
--- @return number Distance
function vector2.distance(v0, v1) end

--- Get squared distance between v0 and v1.
--- @param v0 vector2
--- @param v1 vector2
--- @return number Squared distance
function vector2.distance_squared(v0, v1) end

return vector2