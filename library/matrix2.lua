--- @meta

--- Module for working with matrix2 data.
local matrix2 = {}

--- @class matrix2
--- @field m11 number
--- @field m21 number
--- @field m12 number
--- @field m22 number
matrix2.matrix2 = {}

--- Determinant of matrix.
--- @return number float
function matrix2.matrix2:determinant() end

--- Negates a matrix.
--- @return matrix2 
function matrix2.matrix2:negative() end

--- Transpose of matrix.
--- @return matrix2 
function matrix2.matrix2:transpose() end

--- Cofactor of matrix.
--- @return matrix2 
function matrix2.matrix2:cofactor() end

--- Inverse of matrix.
--- @return matrix2 
function matrix2.matrix2:inverse() end

--- Scales matrix in-place.
--- @param v0 vector2 
--- @return matrix2 
function matrix2.matrix2:scale(v0) end

--- Scales matrix in-place.
--- @param x number 
--- @param y number 
--- @param z number 
--- @return matrix2 
function matrix2.matrix2:scale(x, y, z) end

--- Returns a new matrix2
--- @param m11 number? 
--- @param m21 number? 
--- @param m12 number? 
--- @param m22 number? 
--- @return matrix2 
function matrix2.new(m11, m21, m12, m22) end

--- Returns a matrix with all elements set to zero.
--- @return matrix2 
function matrix2.zero() end

--- Returns an identity matrix.
--- @return matrix2 
function matrix2.identity() end

--- Creates a scaling matrix
--- @param v0 vector2 
--- @return matrix2 
function matrix2.scaling(v0) end

--- Creates a scaling matrix
--- @param x number 
--- @param y number 
--- @param z number 
--- @return matrix2 
function matrix2.scaling(x, y, z) end

--- Linearly interpolate two matrices.
--- @param m0 matrix2 
--- @param m1 matrix2 
--- @param f number 
--- @return matrix2 
function matrix2.lerp(m0, m1, f) end

return matrix2