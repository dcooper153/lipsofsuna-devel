Action = Class()
Action.dict_index = {}
Action.dict_name = {}
Action.dict_press = {}
Action.mods = 0
Action.mouse_sensitivity_x = 0.5
Action.mouse_sensitivity_y = 0.3

--- Registers a new action.
-- @param clss Action class.
-- @param args Arguments.
-- @return Action.
Action.new = function(clss, args)
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

--- Activates all actions that correspond to the event.
-- @param self Action class.
-- @param args Event.
-- @param handle True to handle, false to only update input state.
Action.event = function(clss, args, handle)
	-- Maintain key states.
	if args.type == "keypress" then
		clss.mods = args.mods
		clss.dict_press[args.code] = true
	elseif args.type == "keyrelease" then
		clss.mods = args.mods
		clss.dict_press[args.code] = nil
	end
	-- Activate actions.
	if handle then
		for name,action in pairs(clss.dict_name) do
			action:handle_event(args)
		end
	end
end

--- Returns the name of the control that triggers the action with the requested name.
-- @param self Action class.
-- @param name Action name.
-- @return Key name or nil.
Action.get_control_name = function(self, name)
	local action = self.dict_name[name]
	if not action then return nil end
	if not action.key1 then return nil end
	return "[" .. (Keycode[action.key1] or tostring(action.key1)) .. "]"
end

--- Gets the control response to the event.
-- @param self Action.
-- @param args Event.
-- @return Control response, or nil.
Action.get_event_response = function(self, args)
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
		local res1 = analog("mousex", Action.mouse_sensitivity_x * args.dx)
		local res2 = analog("mousey", Action.mouse_sensitivity_y * args.dy)
		return res1 or res2
	elseif args.type == "mousepress" then
		return digital("mouse" .. args.button, true)
	elseif args.type == "mouserelease" then
		return digital("mouse" .. args.button, false)
	elseif args.type == "mousescroll" then
		return analog("mousez", args.rel < 0 and -1 or 1)
	elseif args.type == "keypress" then
		return digital(args.code, true)
	elseif args.type == "keyrelease" then
		return digital(args.code, false)
	end
end

--- Executes the action if it matches the input event.
-- @param self Action.
-- @param args Event arguments.
-- @return True if handled.
Action.handle_event = function(self, args)
	if not self.enabled then return end
	local resp = self:get_event_response(args)
	if resp == nil then return end
	self.func(resp)
	return true
end

Action:add_getters{
	key1 = function(s) return rawget(s, "__key1") end,
	key2 = function(s) return rawget(s, "__key2") end,
	keys = function(s) return {s.key1, s.key2} end}

Action:add_setters{
	key1 = function(s, v)
		rawset(s, "__key1", v)
	end,
	key2 = function(s, v)
		rawset(s, "__key2", v)
	end,
	keys = function(s, v)
		s.key1 = keys[1]
		s.key2 = keys[2]
	end}
