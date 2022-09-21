local Generator = {}
Generator.__index = Generator

function Generator:Noise(map)
    for y = 1, map.height - 1 do
        for x = 1, map.width - 1 do
            map:SetData(math.random(), x, y)
        end
    end
end

function Generator:Threshold(map, threshold)
    for y = 1, map.height - 1 do
        for x = 1, map.width - 1 do
            local d = map:GetData(x, y)

            if d < threshold then
                d = 0
            else
                d = 1
            end

            map:SetData(d, x, y)
        end
    end
end

return Generator
