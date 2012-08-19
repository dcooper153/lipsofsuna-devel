local Class = require("system/class")

local Input = Class()

--- Registers a new action.
-- @param clss Input class.
-- @return Input.
Input.new = function(clss)
	local self = Class.new(clss)
	self.mods = 0
	self.mouse_sensitivity_x = 0.5
	self.mouse_sensitivity_y = 0.3
	self.pressed = {}
	return self
end

--- Updates the input state.
-- @param self Input.
-- @param args Event.
Input.event = function(self, args)
	-- Maintain key states.
	if args.type == "keypress" then
		args.time = Program.time + 0.25
		self.mods = args.mods
		self.pressed[args.code] = args
	elseif args.type == "keyrelease" then
		self.mods = args.mods
		self.pressed[args.code] = nil
	end
end

--- Returns the human readable name of the key.
-- @param self Input.
-- @param key Key symbol or internal name.
-- @return Key name.
Input.get_key_name = function(self, key)
	if not key then return "----" end
	local mapping = {
		["mousex"] = "mouse x",
		["mousey"] = "mouse y",
		["mousez"] = "wheel",
		["mousez+"] = "wheel+",
		["mousez-"] = "wheel-"}
	return Keycode[key] or mapping[key] or tostring(key)
end

--- Checks if a control is pressed.
-- @param self Input.
-- @param control Control.
-- @param True if pressed, false if not.
Input.is_pressed = function(self, control)
	return self.pressed[control]
end

return Input
