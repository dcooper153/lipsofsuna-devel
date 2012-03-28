Action = Class()
Action.dict_index = {}
Action.dict_key = {}
Action.dict_name = {}
Action.dict_press = {}
Action.mods = 0
Action.mouse_sensitivity_x = 0.5
Action.mouse_sensitivity_y = 0.3

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

Action.event = function(clss, args, list)
	-- Maintain key states.
	if args.type == "keypress" then
		clss.mods = args.mods
		clss.dict_press[args.code] = true
	elseif args.type == "keyrelease" then
		clss.mods = args.mods
		clss.dict_press[args.code] = nil
	end
	-- Activate actions.
	local analog = function(k, v)
		local a = clss.dict_key[k]
		if not a or not a.enabled or (list and not list[a.name]) then return end
		if a.mode == "analog" then a.func(v) end
	end
	local digital = function(k, p)
		local a = clss.dict_key[k]
		if not a or not a.enabled or (list and not list[a.name]) then return end
		if a.mode == "press" and p then a.func()
		elseif a.mode == "toggle" then a.func(p)
		elseif a.mode == "analog" then a.func((not p and 0) or (k == a.key1 and -1 or 1)) end
	end
	if args.type == "mousemotion" then
		analog("mousex", clss.mouse_sensitivity_x * args.dx)
		analog("mousey", clss.mouse_sensitivity_y * args.dy)
	elseif args.type == "mousepress" then
		digital("mouse" .. args.button, true)
	elseif args.type == "mouserelease" then
		digital("mouse" .. args.button, false)
	elseif args.type == "mousescroll" then
		analog("mousez", args.rel < 0 and -1 or 1)
	elseif args.type == "keypress" then
		digital(args.code, true)
	elseif args.type == "keyrelease" then
		digital(args.code, false)
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

Action.get_event_response = function(self, args)
	local analog = function(k, v)
		if self.mode == "analog" then
			if self.key1 == k then
				return v
			end
		end
	end
	local digital = function(k, p)
		if self.mode == "press" and p then
			if self.key1 == k then
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
	-- Activate actions.
	local analog = function(k, v)
		if self.mode == "analog" then
			if self.key1 == k then
				self.func(v)
				return true
			end
		end
	end
	local digital = function(k, p)
		if self.mode == "press" and p then
			if self.key1 == k then
				self.func()
				return true
			end
		elseif self.mode == "toggle" then
			if self.key1 == k then
				self.func(p)
				return true
			end
		elseif self.mode == "analog" then
			if self.key1 == k then
				self.func(p and -1 or 0)
				return true
			elseif self.key2 == k then
				self.func(p and 1 or 0)
				return true
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

Action:add_getters{
	key1 = function(s) return rawget(s, "__key1") end,
	key2 = function(s) return rawget(s, "__key2") end,
	keys = function(s) return {s.key1, s.key2} end}

Action:add_setters{
	key1 = function(s, v)
		if s.key1 == v then return end
		if s.key1 then Action.dict_key[s.key1] = nil end
		if v then
			local a = Action.dict_key[v]
			if a and a.key1 == v then a.key1 = nil end
			if a and a.key2 == v then a.key2 = nil end
			Action.dict_key[v] = s
		end
		rawset(s, "__key1", v)
		-- TODO: Inform binding UI
	end,
	key2 = function(s, v)
		if s.key2 == v then return end
		if s.key2 then Action.dict_key[s.key2] = nil end
		if v then
			local a = Action.dict_key[v]
			if a and a.key1 == v then a.key1 = nil end
			if a and a.key2 == v then a.key2 = nil end
			Action.dict_key[v] = s
		end
		rawset(s, "__key2", v)
		-- TODO: Inform binding UI
	end,
	keys = function(s, v)
		s.key1 = keys[1]
		s.key2 = keys[2]
	end}
