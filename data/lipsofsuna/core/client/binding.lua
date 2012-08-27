--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.client.binding
-- @alias Binding

local Class = require("system/class")

--- TODO:doc
-- @type Binding
local Binding = Class("Binding")
Binding.dict_index = {}
Binding.dict_name = {}

--- Registers a new binding.
-- @param clss Binding class.
-- @param args Arguments.
-- @return Binding.
Binding.new = function(clss, args)
	local self = Class.new(clss)
	local copy = function(k) if args and args[k] then self[k] = args[k] end end
	copy("name")
	copy("mode")
	copy("func")
	copy("mods")
	copy("key1")
	copy("key2")
	self.enabled = not args or args.enabled ~= false
	clss.dict_name[self.name] = self
	table.insert(clss.dict_index, self)
	return self
end

--- Activates all bindings that correspond to the event.
-- @param clss Binding class.
-- @param args Event.
Binding.event = function(clss, args)
	for name,binding in pairs(clss.dict_name) do
		binding:handle_event(args)
	end
end

Binding.find_by_name = function(clss, name)
	return clss.dict_name[name]
end

--- Returns the name of the control that triggers the binding with the requested name.
-- @param self Binding class.
-- @param name Binding name.
-- @return Key name or nil.
Binding.get_control_name = function(self, name)
	local binding = self.dict_name[name]
	if not binding then return nil end
	if not binding.key1 then return nil end
	return "[" .. Client.input:get_key_name(binding.key1) .. "]"
end

--- Gets the control response to the event.
-- @param self Binding.
-- @param args Event.
-- @return Control response, or nil.
Binding.get_event_response = function(self, args)
	local analog = function(k, v)
		if self.mode == "analog" then
			if self.key1 == k then
				return v
			end
		end
	end
	local digital = function(k, p)
		if self.mode == "press" then
			if p and self.key1 == k then
				return true
			end
		elseif self.mode == "toggle" then
			if self.key1 == k then
				return p
			end
		elseif self.mode == "analog" then
			if self.key1 == k then
				return p and -1 or 0
			elseif self.key2 == k then
				return p and 1 or 0
			end
		end
	end
	if args.type == "mousemotion" then
		local res1 = analog("mousex", Client.input.mouse_sensitivity_x * args.dx)
		local res2 = analog("mousey", Client.input.mouse_sensitivity_y * args.dy)
		return res1 or res2
	elseif args.type == "mousepress" then
		return digital("mouse" .. args.button, true)
	elseif args.type == "mouserelease" then
		return digital("mouse" .. args.button, false)
	elseif args.type == "mousescroll" then
		local digi = args.rel < 0 and "mousez-" or "mousez+"
		local res1 = analog("mousez", args.rel < 0 and -1 or 1)
		local res2 = digital(digi, true)
		local res3 = digital(digi, false)
		return res1 or res2 or res3
	elseif args.type == "keypress" then
		return digital(args.code, true)
	elseif args.type == "keyrelease" then
		return digital(args.code, false)
	end
end

--- Executes the binding if it matches the input event.
-- @param self Binding.
-- @param args Event arguments.
-- @return True if handled.
Binding.handle_event = function(self, args)
	if not self.enabled then return end
	local resp = self:get_event_response(args)
	if resp == nil then return end
	self.func(resp)
	return true
end

Binding.is_pressed = function(self)
	if self.key1 and Client.input:is_pressed(self.key1) then return true end
	if self.key2 and Client.input:is_pressed(self.key2) then return true end
end

return Binding


