local Keysym = require("system/keysym")

local Keycode = {}
for k,v in pairs(Keysym) do
	Keycode[v] = k
end

return Keycode
