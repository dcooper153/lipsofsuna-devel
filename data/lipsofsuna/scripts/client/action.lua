Action = Class()
Action.dict_index = {}
Action.dict_key = {}
Action.dict_name = {}
Action.dict_press = {}
Action.mods = 0
Action.mouse_sensitivity_x = 0.5
Action.mouse_sensitivity_y = 0.3

Action.setter = function(self, key, value)
	if key == "key1" then
		if self.key1 ~= value then
			if self.key1 then Action.dict_key[self.key1] = nil end
			if value then
				local a = Action.dict_key[value]
				if a and a.key1 == value then a.key1 = nil end
				if a and a.key2 == value then a.key2 = nil end
				Action.dict_key[value] = self
			end
			Class.setter(self, key, value)
			-- TODO: Inform binding UI
		end
	elseif key == "key2" then
		if self.key2 ~= value then
			if self.key2 then Action.dict_key[self.key2] = nil end
			if value then
				local a = Action.dict_key[value]
				if a and a.key1 == value then a.key1 = nil end
				if a and a.key2 == value then a.key2 = nil end
				Action.dict_key[value] = self
			end
			Class.setter(self, key, value)
			-- TODO: Inform binding UI
		end
	elseif key == "keys" then
		self.key1 = keys[1]
		self.key2 = keys[2]
	else
		Class.setter(self, key, value)
	end
end

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
	if not Player.object then return end
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
	elseif args.type == "keypress" then
		digital(args.code, true)
	elseif args.type == "keyrelease" then
		digital(args.code, false)
	end
end
