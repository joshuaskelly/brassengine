local Action = require("gui.action")

local scripting = {}

local sandbox = {}

local function get_object_in_current_room(id)
    local children = game_screen.view.children

    for i=1, #children do
        local child = children[i]

        if child.id == id then
            return child
        end
    end

    return nil
end

local entrance_intro_text_shown = true

function sandbox.entrance_enter()
    if not entrance_intro_text_shown then
        game_screen:describe("The last thing that you remember is standing before the wizard Lakmir as he waved his hands. Now you find yourself staring at an entryway which lies at the edge of a forest. The Druid's words ring in your ears: \"Within the castle Shadowgate lies your quest. The dreaded warlock lord will use his black magic to raise the behemoth from the dark depths. The combination of his evil arts and the great titan's power will surely destroy us all! You are the last of the line of kings, the seed of prophecy that was foretold eons ago. Only you can stop the evil one from darkening our world forever! Fare thee well.\" Gritting your teeth, you swear by your god's name that you will destroy the warlock lord!")
        entrance_intro_text_shown = true
        return
    end

    game_screen:describe("It's the entrance to Shadowgate. You can hear wolves howling deep in the forest behind you...")
end

function sandbox.entrance_interact(id)
    if id == 2 then
        sandbox.go_to(2)
    else
        game_screen:describe("Nothing happened.")
    end
end

function sandbox.entrance_inspect(id)
    if id == 0 then
        print("ground")
    elseif id == 1 then
        game_screen:describe("You stand before a stone wall that has been carved out of the earth.")
    elseif id == 2 then
        game_screen:describe("The door is now open.")
    elseif id == 3 then
        game_screen:describe("The forest ends some twenty feet from the wall, as if sensing some great evil.")
    elseif id == 4 then
        print("door frame")
    elseif id == 5 then
        print("left grass")
    elseif id == 6 then
        print("right grass")
    elseif id == 7 then
        game_screen:describe("A small hole has been roughly chiselled into the keystone.")
    end
end

local function skull_animate_up(self)
    if self.target.rect.y > 22 then
        self.target.rect.y = self.target.rect.y - 0.25
    else
        self.target:remove_action(self)
    end
end

local function skull_animate_down(self)
    if self.target.rect.y < 34 then
        self.target.rect.y = self.target.rect.y + 0.25
    else
        self.target:remove_action(self)
    end
end

local skull_text_shown = false

function sandbox.entrance_skull_interact()
    local skull = get_object_in_current_room(1)
    if skull == nil then
        return
    end

    if (skull.up) then
        skull:add_action(Action(skull_animate_down))
        skull.up = false
    else
        skull:add_action(Action(skull_animate_up))
        skull.up = true
    end

    if not skull_text_shown then
        game_screen:describe("As if by magic the skull rises.")
        skull_text_shown = true
    end
end

function sandbox.entrance_skull_inspect()
    game_screen:describe("It's the skull of some creature. Its meaning is quite clear: death lurks inside.")
end

function sandbox.entrance_key_interact()
    local key = get_object_in_current_room(2)
    if key == nil then
        return
    end

    key.visible = false
    game_screen:describe("The key is in hand.")
end

function sandbox.entrance_key_inspect()
    game_screen:describe("It's a small iron key.")
end

function sandbox.entrance_door_interact()
    local door = get_object_in_current_room(3)
    if door == nil then
        return
    end

    door.visible = false
    game_screen:describe("The door is open.")
end

function sandbox.entrance_door_inspect()
    game_screen:describe("It's a heavy wooden door with iron hinges.")
end

local hallway_intro_text_shown = false

function sandbox.hallway_enter()
    if not hallway_intro_text_shown then
        game_screen:describe("\"That pitiful wizard Lakmir was a fool to send a buffoon like you to stop me. You will surely regret it for the only thing here for you is a horrible death!\" The sound of maniacal laughter echoes in your ears.")
        hallway_intro_text_shown = true
        return
    end

    game_screen:describe("You stand in a long corridor. Huge stone archways line the entire hall.")
end

function sandbox.hallway_exit_locked()
    game_screen:describe("It's locked.")
end

function sandbox.go_to(room_id, direction)
    game_screen:move_to(room_id, direction)
end

function sandbox.describe(s)
    game_screen:describe(s)
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
