local Rect = {}
Rect.__index = Rect

function Rect.new(x, y, width, height)
    local new_rect = {}
    setmetatable(new_rect, Rect)

    new_rect.x = x
    new_rect.y = y
    new_rect.width = width
    new_rect.height = height

    return new_rect
end

function Rect:contains(x, y)
    if x < self.x then return false end
    if x > self.x + self.width then return false end
    if y < self.y then return false end
    if y > self.y + self.height then return false end

    return true
end

return Rect