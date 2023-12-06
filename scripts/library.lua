--- Generates language server compatible definition files. Output will be
--- written to ./library

local library = {}

library.traverse = {}

local _module = nil

function library.filter(modules)
    for _, module in ipairs(modules) do
        _module = module

        local lines = library.Lines:new()

        local module_header = "--- @meta\n"
        local module_content = library.traverse.module(module)

        lines:concat(module_header)
        lines:concat(module_content)

        local source = lines:join()

        local f = assert(io.open("./library/" .. module.name .. ".lua", "w"))
        f:write(source)
        f:close()
    end
end

function library.traverse.module(module)
    local lines = library.Lines:new()

    local module_summary = "--- " .. string.gsub(module.summary, "\n", "")
    local module_declaration = "local " .. module.name .. " = {}\n"
    local module_contents = library.traverse.contents(module)
    local module_return = "return " .. module.name

    lines:concat(module_summary)
    lines:concat(module_declaration)
    lines:concat(module_contents)
    lines:concat(module_return)

    return lines
end

function library.traverse.contents(module)
    if #module.sections > 0 then
        return library.traverse.sections(module)
    end

    return library.traverse.items(module.items)
end

function library.traverse.items(items)
    local lines = library.Lines:new()

    for _, item in ipairs(items) do
        lines:concat(library.traverse.item(item))
    end

    return lines
end

function library.traverse.item(item)
    if item.type == "function" then
        return library.traverse.func(item)
    end

    return ""
end

function library.traverse.sections(module)
    local lines = library.Lines:new()

    local s = {}

    for _, item in ipairs(module.sections[1].file.items) do
        if item.type == "type" then
            s[item.display_name] = s[item.display_name] or {}
            s[item.display_name].type = item
        elseif item.type == "function" then
            s[item.section] = s[item.section] or {}
            s[item.section].functions = s[item.section].functions or {}
            table.insert(s[item.section].functions, item)
        elseif item.type == "field" then
            s[item.section] = s[item.section] or {}
            s[item.section].fields = s[item.section].fields or {}
            table.insert(s[item.section].fields, item)
        end
    end

    for k, v in pairs(s) do
        lines:concat(library.traverse.section(v))
    end

    return lines
end

function library.traverse.section(section)
    local lines = library.Lines:new()

    if section.type then
        local section_description = string.gsub(section.type.descpription or "", "\n", "")
        local section_tag = "--- @class " .. section.type.name

        lines:concat(section_description)
        lines:concat(section_tag)

        -- Fields
        for _, field in ipairs(section.fields or {}) do
            local field_tag = "--- @field"
            local field_name = string.sub(field.args, 2, #field.args - 1)
            local field_type = field.modifiers.field[1].type
            field_type = library.format.type(field_type)

            local parts = library.Lines:new()

            parts:concat(field_tag)
            parts:concat(field_name)
            parts:concat(field_type)

            lines:concat(parts:join(" "))
        end

        -- Section definition
        local section_definition = _module.name .. "." .. section.type.name .. " = {}\n"
        lines:concat(section_definition)
    end

    -- Functions
    for _, func in ipairs(section.functions or {}) do
        lines:concat(library.traverse.func(func))
    end

    return lines
end

function library.traverse.func(item)
    local lines = library.Lines:new()

    local func_summary = "--- " .. item.summary
    local func_params = library.traverse.func_params(item)
    local func_return = library.traverse.func_return(item)
    local func_signature = "function " .. _module.name .. "." .. item.name .. item.args.." end\n"

    lines:concat(func_summary)
    lines:concat(func_params)
    lines:concat(func_return)
    lines:concat(func_signature)

    return lines
end

function library.traverse.func_params(item)
    local lines = library.Lines:new()

    for _, param_name in ipairs(item.params) do
        local parts = library.Lines:new()

        local param_tag = "--- @param"
        local param_type = library.format.type(item.modifiers.param[param_name].type)
        local param_description = string.gsub(item.params.map[param_name], "\n", "")

        parts:concat(param_tag)
        parts:concat(param_name)
        parts:concat(param_type)
        parts:concat(param_description)

        lines:concat(parts:join(" "))
    end

    return lines
end

function library.traverse.func_return(item)
    local lines = library.Lines:new()

    for _, ret in ipairs(item.retgroups or {}) do
        local parts = library.Lines:new()

        local return_tag = "--- @return"
        local return_type = library.format.type(ret[1].type)
        local return_description = string.gsub(ret[1].text, "\n", "")

        parts:concat(return_tag)
        parts:concat(return_type)
        parts:concat(return_description)

        lines:concat(parts:join(" "))
    end

    return lines
end

library.format = {}

function library.format.type(t)
    if not t then
        return ""
    end

    local optional = false
    local array = false

    if string.sub(t, 1, 1) == "?" then
        t = string.sub(t, 2)
        optional = true
    end

    if string.sub(t, 1, 1) == "{" and string.sub(t, -5)==",...}" then
        t = string.sub(t, 2, #t - 5)
        array = true
    end

    -- Sanitize module.type
    local dot = string.find(t, "%.")
    if dot then
        t = string.sub(t, dot + 1)
    end

    if array then
        t = t .. "[]"
    end

    if optional then
        t = t .. "?"
    end

    return t
end

library.Lines = {}

function library.Lines:new(l)
    local lines = {type="lines"}
    setmetatable(lines, self)
    self.__index = self

    lines:concat(l)

    return lines
end

function library.Lines:concat(other)
    local t = type(other)

    if t == "string" then
        table.insert(self, other)
    elseif t == "table" then
        for _, line in ipairs(other) do
            table.insert(self, line)
        end
    end
end

---@return string
function library.Lines:join(chr)
    chr = chr or "\n"
    return table.concat(self, chr)
end

return library
