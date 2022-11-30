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

local function hide_object_in_current_room(id)
    local object = get_object_in_current_room(id)
    if object == nil then
        return
    end

    object.visible = not object.visible
end

local function open_close_door(id)
    local door = get_object_in_current_room(id)
    if door == nil then
        return
    end

    if door.visible then
        door.visible = false
        game_screen:describe("The door is open.")
    else
        door.visible = true
        game_screen:describe("The door is closed.")
    end
end

--region ENTRANCE

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
    elseif id == 4 then
        sandbox.entrance_door_interact()
    else
        game_screen:describe("Nothing happened.")
    end
end

function sandbox.entrance_inspect(id)
    if id == 0 then
        game_screen:describe("The ground is rocky and bitter. Little thrives here.")
    elseif id == 1 then
        game_screen:describe("You stand before a stone wall that has been carved out of the earth.")
    elseif id == 2 then
        game_screen:describe("The door is now open.")
    elseif id == 3 then
        game_screen:describe("The forest ends some twenty feet from the wall, as if sensing some great evil.")
    elseif id == 4 then
        sandbox.entrance_door_inspect()
    elseif id == 5 then
        -- Left grass
        game_screen:describe("A small plant grows here.")
    elseif id == 6 then
        -- Right grass
        game_screen:describe("A small plant grows here.")
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
    open_close_door(3)
end

function sandbox.entrance_door_inspect()
    game_screen:describe("It's a heavy wooden door with iron hinges.")
end

--endregion

--region HALLWAY

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

function sandbox.hallway_interact(id)
    if id == 2 then
        sandbox.go_to(4)
    elseif id == 3 then
        sandbox.go_to(3, "right")
    elseif id == 4 then
        sandbox.hallway_door_1_interact()
    elseif id == 5 then
        sandbox.hallway_door_2_interact()
    else
        game_screen:describe("Nothing happened.")
    end
end

function sandbox.hallway_inspect(id)
    if id == 1 then
    elseif id == 2 then
        game_screen:describe("The door is now open.")
    elseif id == 3 then
        game_screen:describe("The door is now open.")
    elseif id == 4 then
        sandbox.hallway_door_1_inspect()
    elseif id == 5 then
        sandbox.hallway_door_2_inspect()
    else
    end
end

function sandbox.hallway_rug_interact(id)
    game_screen:describe("Nothing happened.")
end

function sandbox.hallway_rug_inspect()
    game_screen:describe("It's a beautifully woven rug.")
end

function sandbox.hallway_door_1_interact()
    open_close_door(7)
end

function sandbox.hallway_door_1_inspect()
    game_screen:describe("This wooden door is reinforced with heavy sheets of steel.")
end

function sandbox.hallway_door_2_interact()
    open_close_door(8)
end

function sandbox.hallway_door_2_inspect()
    game_screen:describe("Even though this door is only an inch thick, it is very sturdy.")
end

--endregion

--region CLOSET

local closet_intro_text_shown = false

function sandbox.closet_enter()
    if not closet_intro_text_shown then
        game_screen:describe("As you enter, you can see a broom and bucket inside.")
        closet_intro_text_shown = true
    else
        game_screen:describe("You are in a small cramped closet.")
    end
end

--endregion

--region Conrad

function sandbox.conrad_interact(id)
end

function sandbox.conrad_inspect(id)
    if id == 1 then
        game_screen:describe("The creature moves towards you.")
    end
end

--endregion

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
