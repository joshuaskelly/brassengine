--- @meta

--- Module for working with matrix3 data.
local matrix3 = {}

--- @class matrix3
--- @field m11 number
--- @field m21 number
--- @field m31 number
--- @field m12 number
--- @field m22 number
--- @field m32 number
--- @field m13 number
--- @field m23 number
--- @field m33 number
matrix3.matrix3 = {}

--- Determinant of matrix.
--- @return number float
function matrix3.matrix3:determinant() end

--- Negates a matrix.
--- @return matrix3 
function matrix3.matrix3:negative() end

--- Transpose of matrix.
--- @return matrix3 
function matrix3.matrix3:transpose() end

--- Cofactor of matrix.
--- @return matrix3 
function matrix3.matrix3:cofactor() end

--- Inverse of matrix.
--- @return matrix3 
function matrix3.matrix3:inverse() end

--- Scales matrix in-place.
--- @param v0 vector3 
--- @return matrix3 
function matrix3.matrix3:scale(v0) end

--- Scales matrix in-place.
--- @param x number 
--- @param y number 
--- @param z number 
--- @return matrix3 
function matrix3.matrix3:scale(x, y, z) end

--- Returns a new matrix3
--- @param m11 number? 
--- @param m21 number? 
--- @param m31 number? 
--- @param m12 number? 
--- @param m22 number? 
--- @param m32 number? 
--- @param m13 number? 
--- @param m23 number? 
--- @param m33 number? 
--- @return matrix3 
function matrix3.new(m11, m21, m31, m12, m22, m32, m13, m23, m33) end

--- Returns a matrix with all elements set to zero.
--- @return matrix3 
function matrix3.zero() end

--- Returns an identity matrix.
--- @return matrix3 
function matrix3.identity() end

--- Creates a rotation matrix
--- @param q0 quaternion 
--- @return matrix3 
function matrix3.rotation(q0) end

--- Creates a scaling matrix
--- @param v0 vector3 
--- @return matrix3 
function matrix3.scaling(v0) end

--- Creates a scaling matrix
--- @param x number 
--- @param y number 
--- @param z number 
--- @return matrix3 
function matrix3.scaling(x, y, z) end

--- Linearly interpolate two matrices.
--- @param m0 matrix3 
--- @param m1 matrix3 
--- @param f number 
--- @return matrix3 
function matrix3.lerp(m0, m1, f) end

return matrix3