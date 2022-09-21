local Map = {}
Map.__index = Map

function Map.new(width, height)
    local new_map = {}
    setmetatable(new_map, Map)

    new_map.width = width
    new_map.height = height
    new_map.data = {}

    for i = 1, width * height do
        new_map.data[i] = 0
    end

    return new_map
end

function Map:Contains(x, y)
    if x < 0 then
        return false
    elseif x >= self.width then
        return false
    elseif y < 0 then
        return false
    elseif y >= self.height then
        return false
    end

    return true
end

function Map:GetData(x, y)
    return self.data[y * self.width + x]
end

function Map:SetData(d, x, y)
    self.data[y * (self.width) + x] = d
end

function Map:IsSolid(x, y)
    return self:GetData(x, y) == 1
end

return Map
