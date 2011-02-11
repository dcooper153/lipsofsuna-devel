local oldinst = Object.new
local oldgetter = Object.getter
local oldsetter = Object.setter
local oldfind = Object.find
Object.objects = {}
Object.load_meshes = true
setmetatable(Object.objects, {__mode = "v"})

Object.getter = function(self, key)
	if key == "model_name" then
		return self.model and self.model.name or ""
	else
		return oldgetter(self, key)
	end
end

Object.setter = function(self, key, value)
	if key == "model" and type(value) == "string" then
		local m = Model:load{file = value, mesh = self.load_meshes}
		oldsetter(self, key, m)
	else
		oldsetter(self, key, value)
	end
end

--- Creates a new object.
-- @param clss Object class.
-- @param args Arguments.<ul>
--   <li>id: Unique object ID.</li></ul>
-- @return New object.
Object.new = function(clss, args)
	-- Create an object with the desired ID.
	local self = oldinst(clss, args)
	self.id = args and args.id or clss:get_free_id()
	-- If there was an existing object with the same ID, hide it. This can
	-- happen when object was hidden and displayed again before it being GC'd.
	local old = Object.objects[self.id]
	if old then old.realized = false end
	Object.objects[self.id] = self
	-- Repeat particle animations by default.
	if self.particle_animation then
		self:particle_animation{loop = true}
	end
	return self
end

--- Gets a free object ID.
-- @param clss Object class.
-- @return Free object ID.
Object.get_free_id = function(clss)
	while true do
		local id = math.random(0x1000000, 0x7FFFFFF)
		if not Object:find{id = id} then
			return id
		end
	end
end

Object.find = function(self, args)
	if args.id then
		-- Search by ID.
		local obj = Object.objects[args.id]
		if not obj then return end
		-- Optional distance check.
		if args.point and args.radius then
			if not obj.realized then return end
			if (obj.position - args.point).length > args.radius then return end
		end
		return obj
	else
		-- Search by position or sector.
		local ret = oldfind(self, args)
		-- Create an easily searchable dictionary.
		local dict = {}
		for k,v in pairs(ret) do
			dict[v.id] = v
		end
		return dict
	end
end
