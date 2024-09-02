--- @meta

--- Module for working with matrix4 data.
local matrix4 = {}

--- Returns a new matrix4
--- @param m11 number? 
--- @param m21 number? 
--- @param m31 number? 
--- @param m41 number? 
--- @param m12 number? 
--- @param m22 number? 
--- @param m32 number? 
--- @param m42 number? 
--- @param m13 number? 
--- @param m23 number? 
--- @param m33 number? 
--- @param m43 number? 
--- @param m14 number? 
--- @param m24 number? 
--- @param m34 number? 
--- @param m44 number? 
--- @return matrix4 
function matrix4.new(m11, m21, m31, m41, m12, m22, m32, m42, m13, m23, m33, m43, m14, m24, m34, m44) end

--- Returns a matrix with all elements set to zero.
--- @return matrix4 
function matrix4.zero() end

--- Returns an identity matrix.
--- @return matrix4 
function matrix4.identity() end

--- Creates a rotation matrix
--- @param q0 quaternion 
--- @return matrix4 
function matrix4.rotation(q0) end

--- Creates a translation matrix
--- @param v0 vector3 
--- @return matrix4 
function matrix4.translation(v0) end

--- Creates a translation matrix
--- @param x number 
--- @param y number 
--- @param z number 
--- @return matrix4 
function matrix4.translation(x, y, z) end

--- Creates a scaling matrix
--- @param v0 vector3 
--- @return matrix4 
function matrix4.scaling(v0) end

--- Creates a scaling matrix
--- @param x number 
--- @param y number 
--- @param z number 
--- @return matrix4 
function matrix4.scaling(x, y, z) end

--- Linearly interpolate two matrices.
--- @param m0 matrix4 
--- @param m1 matrix4 
--- @param f number 
--- @return matrix4 
function matrix4.lerp(m0, m1, f) end

--- Creates a look at transformation.
--- @param position vector3 
--- @param target vector3 
--- @param up vector3 
--- @return matrix4 
function matrix4.look_at(position, target, up) end

--- Creates an orthographic view transformation.
--- @param l number 
--- @param r number 
--- @param b number 
--- @param t number 
--- @param n number 
--- @param f number 
--- @return matrix4 
function matrix4.ortho(l, r, b, t, n, f) end

--- Creates an perspective view transformation.
--- @param fov_y number 
--- @param aspect number 
--- @param n number 
--- @param f number 
--- @return matrix4 
function matrix4.perspective(fov_y, aspect, n, f) end

--- Creates an perspective view transformation.
--- @param fov_y number 
--- @param w number 
--- @param h number 
--- @param n number 
--- @param f number 
--- @return matrix4 
function matrix4.perspective_fov(fov_y, w, h, n, f) end

--- Creates an perspective view transformation.
--- @param fov_y number 
--- @param aspect number 
--- @param n number 
--- @return matrix4 
function matrix4.perspective_infinite(fov_y, aspect, n) end

--- @class matrix4
--- @field m11 number
--- @field m21 number
--- @field m31 number
--- @field m41 number
--- @field m12 number
--- @field m22 number
--- @field m32 number
--- @field m42 number
--- @field m13 number
--- @field m23 number
--- @field m33 number
--- @field m43 number
--- @field m14 number
--- @field m24 number
--- @field m34 number
--- @field m44 number
matrix4.matrix4 = {}

--- Determinant of matrix.
--- @return number float
function matrix4.matrix4:determinant() end

--- Negates a matrix.
--- @return matrix4 
function matrix4.matrix4:negative() end

--- Transpose of matrix.
--- @return matrix4 
function matrix4.matrix4:transpose() end

--- Cofactor of matrix.
--- @return matrix4 
function matrix4.matrix4:cofactor() end

--- Inverse of matrix.
--- @return matrix4 
function matrix4.matrix4:inverse() end

--- Translates matrix in-place.
--- @param v0 vector3 
--- @return matrix4 
function matrix4.matrix4:translate(v0) end

--- Translates matrix in-place.
--- @param x number 
--- @param y number 
--- @param z number 
--- @return matrix4 
function matrix4.matrix4:translate(x, y, z) end

--- Scales matrix in-place.
--- @param v0 vector3 
--- @return matrix4 
function matrix4.matrix4:scale(v0) end

--- Scales matrix in-place.
--- @param x number 
--- @param y number 
--- @param z number 
--- @return matrix4 
function matrix4.matrix4:scale(x, y, z) end

return matrix4