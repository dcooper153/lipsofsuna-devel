__userdata_lookup = setmetatable({}, {__mode = "kv"})

local class_meta = {
	__call = function(self, ...)
		return self:new(...)
	end,
	__index = function(self, k)
		local p = self
		repeat
			-- Getter access.
			local t = rawget(p, "class_getters")
			if t then
				local f = t[k]
				if f then return f(self) end
			end
			-- Class access.
			local v = rawget(p, k)
			if v then return v end
			p = rawget(p, "super")
		until not p
	end,
	__newindex = function(self, k, v)
		-- Setter access.
		local t = rawget(self, "class_setters")
		if t then
			local f = t[k]
			if f then return f(self, v) end
		end
		-- Class access.
		rawset(self, k, v)
	end,
	__tostring = function(self)
		return "class:" .. (self.class_name or "???")
	end}

local data_meta = {
	__add = function(self, v)
		local c = rawget(self, "class")
		return c.__add(self, v)
	end,
	__index = function(self, k)
		-- Getter access.
		local c = rawget(self, "class")
		local t = rawget(c, "getters")
		if t then
			local f = t[k]
			if f then return f(self) end
		end
		-- Data access.
		local v = rawget(self, k)
		if v then return v end
		-- Class access.
		return c[k]
	end,
	__mul = function(self, v)
		local c = rawget(self, "class")
		return c.__mul(self, v)
	end,
	__newindex = function(self, k, v)
		-- Setter access.
		local c = rawget(self, "class")
		local t = rawget(c, "setters")
		if t then
			local f = t[k]
			if f then return f(self, v) end
		end
		-- Data access.
		rawset(self, k, v)
	end,
	__sub = function(self, v)
		local c = rawget(self, "class")
		return c.__sub(self, v)
	end,
	__tostring = function(self)
		local c = rawget(self, "class")
		if c.__str then return c.__str(self) end
		return "data:" .. (self.class.class_name or "???")
	end}

Class = setmetatable({class_name = "Class"}, class_meta)

--- Creates a new class or an instance of a class.
-- @param clss Class class.
-- @param args Base class for classes, initialization arguments for instances.
Class.new = function(clss, base)
	if clss == Class then
		local super = base or Class
		local self = setmetatable({super = super}, class_meta)
		if super.getters then
			self.getters = {}
			for k,v in pairs(super.getters) do self.getters[k] = v end
		end
		if super.setters then
			self.setters = {}
			for k,v in pairs(super.setters) do self.setters[k] = v end
		end
		return self
	else
		local self = setmetatable({class = clss}, data_meta)
		if base then
			for k,v in pairs(base) do self[k] = v end
		end
		return self
	end
end

Class.add_class_getters = function(clss, args)
	local s = {}
	if clss.class_getters then
		for k,v in pairs(clss.class_getters) do s[k] = v end
	end
	for k,v in pairs(args) do s[k] = v end
	clss.class_getters = s
end

Class.add_class_setters = function(clss, args)
	local s = {}
	if clss.class_setters then
		for k,v in pairs(clss.class_setters) do s[k] = v end
	end
	for k,v in pairs(args) do s[k] = v end
	clss.class_setters = s
end

Class.add_getters = function(clss, args)
	local s = {}
	if clss.getters then
		for k,v in pairs(clss.getters) do s[k] = v end
	end
	for k,v in pairs(args) do s[k] = v end
	clss.getters = s
end

Class.add_setters = function(clss, args)
	local s = {}
	if clss.setters then
		for k,v in pairs(clss.setters) do s[k] = v end
	end
	for k,v in pairs(args) do s[k] = v end
	clss.setters = s
end
