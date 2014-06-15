--- Input binding management.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.input.input
-- @alias Input

local Binding = require("core/input/binding")
local Class = require("system/class")
local ControlsSerializer = require("core/input/controls-serializer")
local Hooks = require("system/hooks")
local Keycode = require("system/keycode")
local Time = require("system/time")

--- Input binding management.
-- @type Input
local Input = Class("Input")

--- Registers a new action.
-- @param clss Input class.
-- @return Input.
Input.new = function(clss)
	local self = Class.new(clss)
	self.mods = 0
	self.mouse_motion_max = 150
	self.mouse_sensitivity_x = 0.5
	self.mouse_sensitivity_y = 0.3
	self.pressed = {}
	self.bindings = Binding --FIXME
	self.serializer = ControlsSerializer()
	return self
end

--- Loads the input bindings.
-- @param self Input.
Input.load = function(self)
	self.serializer:load(self.bindings)
end

--- Updates the input state.
-- @param self Input.
-- @param args Event.
Input.event = function(self, args)
	-- Filter out insane jumps in mouse input.
	if args.type == "mousemotion" then
		local m = self.mouse_motion_max
		args.dx = math.max(args.dx, -m)
		args.dx = math.min(args.dx, m)
		args.dy = math.max(args.dy, -m)
		args.dy = math.min(args.dy, m)
	end
	-- Maintain key states.
	if args.type == "keypress" then
		args.time = Time:get_secs() + 0.25
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
-- @return True if pressed, false if not.
Input.is_pressed = function(self, control)
	return self.pressed[control]
end

--- Registers an input binding.
-- @param self Input.
-- @param args Arguments.
-- @return Binding.
Input.register_binding = function(self, args)
	local func = args.func
	local binding = self.bindings.dict_name[args.name]
	if not binding then
		args.func = function(...) binding.hooks:call(...) end
		binding = Binding(args)
		binding.hooks = Hooks()
	end
	binding.hooks:register(0, func)
	return binding
end

--- Saves the input bindings.
-- @param self Input.
Input.save = function(self)
	self.serializer:save(self.bindings)
end

return Input
