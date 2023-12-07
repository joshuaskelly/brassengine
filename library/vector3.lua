--- @meta

--- Module for working with vector3 data.
local vector3 = {}

--- Returns a new vector3
--- @param x number 
--- @param y number 
--- @param z number 
--- @return vector3 
function vector3.new(x, y, z) end

--- Returns a vector made from snapping the components to given resolution.
--- @param v0 vector3 
--- @param f number  Resolution of snap
--- @return vector3 
function vector3.snap(v0, f) end

--- Negates a vector.
--- @param v0 vector3 
--- @return vector3 
function vector3.negative(v0) end

--- Returns a vector made from the absolute values of the components.
--- @param v0 vector3 
--- @return vector3 
function vector3.abs(v0) end

--- Returns a vector made from the floor of the components.
--- @param v0 vector3 
--- @return vector3 
function vector3.floor(v0) end

--- Returns a vector made from the ceil of the components.
--- @param v0 vector3 
--- @return vector3 
function vector3.ceil(v0) end

--- Returns a vector made from rounding the components.
--- @param v0 vector3 
--- @return vector3 
function vector3.round(v0) end

--- Returns a vector that is the component-wise max of v0 and v1.
--- @param v0 vector3 
--- @param v1 vector3 
--- @return vector3 
function vector3.max(v0, v1) end

--- Returns a vector that is the component-wise min of v0 and v1.
--- @param v0 vector3 
--- @param v1 vector3 
--- @return vector3 
function vector3.max(v0, v1) end

--- Returns a vector that is a component-wise clamp of v0 such that min < v0 < max.
--- @param v0 vector3  Vector to clamp
--- @param min vector3  Min vector
--- @param max vector3  Max vector
--- @return vector3 
function vector3.clamp(v0, min, max) end

--- Cross product of two vectors.
--- @param v0 vector3 
--- @param v1 vector3 
--- @return vector3 
function vector3.cross(v0, v1) end

--- Returns a vector in the direction of v0 with magnitude 1.
--- @param v0 vector3 
--- @return vector3 
function vector3.normalize(v0) end

--- Dot product of two vectors.
--- @param v0 vector3 
--- @param v1 vector3 
--- @return number float
function vector3.dot(v0, v1) end

--- Project v0 onto v1.
--- @param v0 vector3 
--- @param v1 vector3 
--- @return vector3 
function vector3.project(v0, v1) end

--- Reflect v0 off of plane given by normal.
--- @param v0 vector3 
--- @param normal vector3  Plane normal
--- @return vector3 
function vector3.reflect(v0, normal) end

--- Rotate v0 around ra clockwise by angle in radians.
--- @param v0 vector3 
--- @param ra vector3  Vector to rotate around
--- @param radians number  Angle to rotate in radians
--- @return vector3 
function vector3.rotate(v0, ra, radians) end

--- Linearly interpolate between v0 and v1.
--- @param v0 vector3 
--- @param v1 vector3 
--- @param t number  Value used to interpolate between v0 and v1.
--- @return vector3 
function vector3.lerp(v0, v1, t) end

--- Get vector magnitude.
--- @param v0 vector3 
--- @return number float
function vector3.length(v0) end

--- Get vector squared magnitude.
--- @param v0 vector3 
--- @return number float
function vector3.length_squared(v0) end

--- Get distance between v0 and v1.
--- @param v0 vector3 
--- @param v1 vector3 
--- @return number float
function vector3.distance(v0, v1) end

--- Get squared distance between v0 and v1.
--- @param v0 vector3 
--- @param v1 vector3 
--- @return number float
function vector3.distance_squared(v0, v1) end

--- @class vector3
--- @field length number
--- @field x number
--- @field y number
--- @field z number
vector3.vector3 = {}

return vector3