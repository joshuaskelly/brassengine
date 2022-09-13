-- Testing blitting stuff
local draw = require("draw")

-- Called once at startup
function _init()
    print("lua init")
    frame = 0
end

-- Called once per frame
function _update()
    frame = frame + 1
end

-- Called once per frame
function _draw()
    draw.clear(0)
    local x, y = mouse_position()
    draw.test_blit(0, 0, x, y, 0, 0, x, y)
    local coords = string.format("(%i,%i)", x, y)

    local w = #coords * 8
    if x > (320 - w) then
        x = 320 - w
    end

    if y > 192 then
        y = 192
    end

    text(coords, x, y)
    text(string.format("frame:%i", frame), 0, 192)
end

function sprite(index, x, y)
    sx = (index % 32) * 8
    sy = (index // 32) * 8
    draw.test_blit(sx, sy, 8, 8, x, y, 8, 8)
end

function text(message, x, y)
    for i = 1, string.len(message) do
        local c = string.sub(message, i, i)
        local index = byte_to_index[c]
        sprite(index, x + ((i - 1) * 8), y)
    end
end

byte_to_index = {}
byte_to_index[" "] = 832
byte_to_index["a"] = 833
byte_to_index["b"] = 834
byte_to_index["c"] = 835
byte_to_index["d"] = 836
byte_to_index["e"] = 837
byte_to_index["f"] = 838
byte_to_index["g"] = 839
byte_to_index["h"] = 840
byte_to_index["i"] = 841
byte_to_index["j"] = 842
byte_to_index["k"] = 843
byte_to_index["l"] = 844
byte_to_index["m"] = 845
byte_to_index["n"] = 846
byte_to_index["o"] = 847
byte_to_index["p"] = 848
byte_to_index["q"] = 849
byte_to_index["r"] = 850
byte_to_index["s"] = 851
byte_to_index["t"] = 852
byte_to_index["u"] = 853
byte_to_index["v"] = 854
byte_to_index["w"] = 855
byte_to_index["x"] = 856
byte_to_index["y"] = 857
byte_to_index["z"] = 858
byte_to_index["A"] = 833
byte_to_index["B"] = 834
byte_to_index["C"] = 835
byte_to_index["D"] = 836
byte_to_index["E"] = 837
byte_to_index["F"] = 838
byte_to_index["G"] = 839
byte_to_index["H"] = 840
byte_to_index["I"] = 841
byte_to_index["J"] = 842
byte_to_index["K"] = 843
byte_to_index["L"] = 844
byte_to_index["M"] = 845
byte_to_index["N"] = 846
byte_to_index["O"] = 847
byte_to_index["P"] = 848
byte_to_index["Q"] = 849
byte_to_index["R"] = 850
byte_to_index["S"] = 851
byte_to_index["T"] = 852
byte_to_index["U"] = 853
byte_to_index["V"] = 854
byte_to_index["W"] = 855
byte_to_index["X"] = 856
byte_to_index["Y"] = 857
byte_to_index["Z"] = 858
byte_to_index["?"] = 859
byte_to_index["!"] = 860
byte_to_index["."] = 861
byte_to_index[","] = 862
byte_to_index["-"] = 863
byte_to_index["+"] = 864
byte_to_index[":"] = 865
byte_to_index[";"] = 866
byte_to_index["\""] = 867
byte_to_index["("] = 868
byte_to_index[")"] = 869
byte_to_index["@"] = 870
byte_to_index["&"] = 871
byte_to_index["1"] = 872
byte_to_index["2"] = 873
byte_to_index["3"] = 874
byte_to_index["4"] = 875
byte_to_index["5"] = 876
byte_to_index["6"] = 877
byte_to_index["7"] = 878
byte_to_index["8"] = 879
byte_to_index["9"] = 880
byte_to_index["0"] = 881
byte_to_index["%"] = 882
byte_to_index["^"] = 883
byte_to_index["*"] = 884
byte_to_index["{"] = 885
byte_to_index["}"] = 886
byte_to_index["="] = 887
byte_to_index["#"] = 888
byte_to_index["/"] = 889
byte_to_index["\\"] = 890
byte_to_index["$"] = 891
byte_to_index["["] = 893
byte_to_index["]"] = 894
byte_to_index["<"] = 895
byte_to_index[">"] = 896
byte_to_index["'"] = 897
byte_to_index["`"] = 898
byte_to_index["~"] = 899