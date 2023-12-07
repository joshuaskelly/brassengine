--- @meta

--- Module for working with quaternion data.
local quaternion = {}

--- Returns a new quaternion.
--- @param x number 
--- @param y number 
--- @param z number 
--- @param w number 
--- @return quaternion 
function quaternion.new(x, y, z, w) end

--- Creates quaternion with same rotation, but magnitude of 1.
--- @param q0 quaternion 
--- @return quaternion 
function quaternion.normalize(q0) end

--- Dot product of two quaternions.
--- @param q0 quaternion 
--- @param q1 quaternion 
--- @return number float
function quaternion.normalize(q0, q1) end

--- Creates a rotation from a unit vector and angle to rotate around the vector.
--- @param v0 vector3 
--- @param radians number 
--- @return quaternion 
function quaternion.from_axis_angle(v0, radians) end

--- Creates a rotation from v0 to v1.
--- @param v0 vector3 
--- @param v1 vector3 
--- @return quaternion 
function quaternion.from_vector3(v0, v1) end

--- Create a rotation from a rotation matrix.
--- @param m0 matrix4 
--- @return quaternion 
function quaternion.from_matrix4(m0) end

--- Create a rotation from Euler angles.
--- @param v0 vector3 
--- @return quaternion 
function quaternion.from_euler(v0) end

--- Create a rotation from Euler angles.
--- @param x number 
--- @param y number 
--- @param z number 
--- @return quaternion 
function quaternion.from_euler(x, y, z) end

--- Return Euler angle representation of rotation.
--- @param q0 quaternion 
--- @return vector3 
function quaternion.to_euler(q0) end

--- Linearly interpolate between q0 and q1.
--- @param q0 quaternion 
--- @param q1 quaternion 
--- @param t number  Value used to interpolate between q0 and q1.
--- @return quaternion 
function quaternion.lerp(q0, q1, t) end

--- Linearly interpolate between q0 and q1, using spherical linear interpolation.
--- @param q0 quaternion 
--- @param q1 quaternion 
--- @param t number  Value used to interpolate between q0 and q1.
--- @return quaternion 
function quaternion.slerp(q0, q1, t) end

--- Get angle between two rotations
--- @param q0 quaternion 
--- @param q1 quaternion 
--- @return number float
function quaternion.angle(q0, q1) end

--- @class quaternion
--- @field x number
--- @field y number
--- @field z number
--- @field w number
quaternion.quaternion = {}

--- Conjugate of quaternion.
--- @return quaternion 
function quaternion.quaternion:conjugate() end

--- Inverse of quaternion.
--- @return quaternion 
function quaternion.quaternion:inverse() end

--- Get quaternion magnitude
--- @return number float
function quaternion.quaternion:length() end

--- Get quaternion squared magnitude
--- @return number float
function quaternion.quaternion:length_squared() end

return quaternion