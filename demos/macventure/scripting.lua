local scripting = {}

local sandbox = {}

function sandbox.entrance_skull_default()
    game_screen:describe("It's the skull of some creature. Its meaning is quite clear: death lurks inside.")
end

function sandbox.entrance_key_default()
    game_screen:describe("It's a small iron key.")
end

function sandbox.entrance_door_default()
    game_screen:describe("It's a heavy wooden door with iron hinges.")
end

function scripting.execute(s)
    if s == nil then
        return
    end

    local func = load(s, nil, nil, sandbox)

    if func == nil then
        print("Failed to execute script: "..s)
        return
    end

    func()
end

return scripting
