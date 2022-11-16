local GUI = require("gui")
local Text = require("gui.text")

local function split(s, pattern)
    local result = {}
    for c in string.gmatch(s, pattern) do
        table.insert(result, c)
    end

    return result
end

local function join(s, pattern)
    local result = s[1]

    for i = 2, #s do
        result = result..pattern..s[i]
    end

    return result
end

local function slice(a, start, end_)
    if end_ == nil then
        end_ = #a
    end

    local result = {}

    for i = start, end_ do
        table.insert(result, a[i])
    end

    return result
end

local DescriptionText = {}
DescriptionText.__index = DescriptionText

setmetatable(DescriptionText, {
    __index = GUI,
    __call = function(cls, ...)
        local self = setmetatable({}, cls)
        self:_init(...)

        return self
    end
})

function DescriptionText:_init(x, y)
    GUI._init(self, x, y, 224, 40)

    self.text_object = Text("", 0, 0)
    self.text = ""
    self.time = time()
    self.lines = {}
    self.current_line = 1
    self.lines_to_show = 2
    self.done = true

    self:set("The last thing that you \nremember is standing before \nthe wizard Lakmir as he \nwaved his hands.")
    self:add_child(self.text_object)
end

function DescriptionText:update()
    GUI.update(self)

    local chars = (time() - self.time) * 16 // 1000

    self.text_object:set(string.sub(self.text, 0, chars))
end

function DescriptionText:contains(x, y)
    -- Consume all clicks while showing text
    if not self.done then
        return true
    end

    return GUI.contains(self, x, y)
end

function DescriptionText:on_click(x, y)
    local char = (time() - self.time) * 16 // 1000

    if char < #self.text then
        -- Show the full text
        self.time = -100000
    else
        -- Go to next page
        self:advance()
    end

    return true
end

local function lines(text)
    local words = split(text, "%S+")
    local line = ""
    local result = {}

    for _, word in ipairs(words) do
        if #line == 0 then
            line = word
        elseif (#line + #word + 1) < 28 then
            line = line.." "..word
        else
            table.insert(result, line)
            line = word
        end
    end

    if #line > 0 then
        table.insert(result, line)
    end

    return result
end

function DescriptionText:set(text)
    self.done = false
    self.lines = lines(text)

    self.current_line = 1 - self.lines_to_show
    self:advance()
end

function DescriptionText:advance()
    if self.current_line + self.lines_to_show >= #self.lines then
        self.done = true
        self.text = ""
        return
    end

    self.current_line = self.current_line + self.lines_to_show
    self.text = join(slice(self.lines, self.current_line, self.current_line + self.lines_to_show - 1), "\n")
    self.text_object:set("")
    self.time = time()
end

return DescriptionText
