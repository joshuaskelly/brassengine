local function print_function(item, mod)
    print("--- "..item.summary)

    for _, param in ipairs(item.params) do
        local t = item.modifiers.param[param].type or ""
        print("--- @param "..param.." "..t..item.params.map[param])
    end

    for _, ret in ipairs(item.retgroups) do
        local r = ret[1]
        print("--- @return "..(r.type and r.type.." " or "")..r.text)
    end

    print("function "..mod.name.."."..item.name.."("..table.concat(item.params, ", ")..") end")
end

local function print_item(item, mod)
    if item.type == "function" then
        print_function(item, mod)
    end
end

return {
    filter = function(t)
        for _, mod in ipairs(t) do
            print("---@meta")
            print()
            print("local "..mod.name.." = {}")
            print()

            for _, item in ipairs(mod.items) do
                print_item(item, mod)
                print()
            end

            print("return "..mod.name)
        end
    end
}
