require "system/class"
require "system/core"
require "system/model"

if not Los.program_load_extension("object") then
	error("loading extension `object' failed")
end

------------------------------------------------------------------------------

Object = Class()
Object.class_name = "Object"
Object.objects = setmetatable({}, {__mode = "v"})
Object.dict_active = setmetatable({}, {__mode = "k"})

--- Creates a new object.
-- @param clss Object class.
-- @param args Arguments.<ul>
--   <li>id: Unique object ID.</li></ul>
-- @return New object.
Object.new = function(clss, args)
	-- Create the object.
	local self = Class.new(clss)
	self.handle = Los.object_new()
	__userdata_lookup[self.handle] = self
	-- Select the unique ID.
	-- If there was an existing object with the same ID, hide it. This can
	-- happen when object was hidden and displayed again before being GC'd.
	self.id = args and args.id or clss:get_free_id()
	local old = clss.objects[self.id]
	if old then old.realized = false end
	clss.objects[self.id] = self
	-- Copy arguments.
	if args then
		for k,v in pairs(args) do
			if k ~= "realized" then self[k] = v end
		end
		if args.realized then self.realized = true end
	end
	return self
end

--- Finds objects.
-- @param clss Object class.
-- @param args Arguments.<ul>
--   <li>id: Object ID for ID search.</li>
--   <li>point: Center point for radius search.</li>
--   <li>radius: Search radius for radius search.</li>
--   <li>sector: Return all object in this sector.</li></ul>
-- @return Table of matching objects.
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
		local list = Los.object_find{
			point = args.point and args.point.handle,
			radius = args.radius,
			sector = args.sector}
		for k,v in pairs(list) do list[k] = __userdata_lookup[v] end
		-- Create an easily searchable dictionary.
		local dict = {}
		for k,v in pairs(list) do
			dict[v.id] = v
		end
		return dict
	end
end

--- Gets a free object ID.
-- @param clss Object class.
-- @return Free object ID.
Object.get_free_id = function(clss)
	while true do
		local id = math.random(0x1000000, 0x7FFFFFF)
		if not clss:find{id = id} then
			return id
		end
	end
end

--- Adds the object to the list of active objects.
-- @param self Objects.
-- @param secs Number of seconds to remain active.
Object.activate = function(self, secs)
	Object.dict_active[self] = secs
end

--- Recalculates the bounding box of the model of the object.
-- @param self Object.
Object.calculate_bounds = function(self)
	Los.object_calculate_bounds(self.handle)
end

--- Detaches the object from the scene.
-- @param self Object.
Object.detach = function(self)
	self.realized = false
	Object.dict_active[self] = nil
end

--- Prevents map sectors around the object from being unloaded.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>radius: Refresh radius.</li></ul>
Object.refresh = function(self, args)
	Los.object_refresh(self.handle, args)
end

--- The local bounding box of the object.
-- @name Object.bounding_box
-- @class table

--- The local center offset of the bounding box of the object.
-- @name Object.center_offset
-- @class table

--- The unique ID of the object.
-- @name Object.id
-- @class table

--- The model of the object.
-- @name Object.model
-- @class table

--- The position of the object.
-- @name Object.position
-- @class table

--- The realization status of the object.
-- @name Object.realized
-- @class table

--- The rotation of the object.
-- @name Object.rotation
-- @class table

--- The map sector of the object (read-only).
-- @name Object.sector
-- @class table

--- The static object status of the object.
--
-- Static objects are not affected by the regular sector loading and unloading
-- scheme. They will persist and retain their rendering and physics status even
-- after the sector has been unloaded. On the contrary, non-static objects
-- cannot remain visible if the sector in which they are is hidden.
--
-- @name Object.static
-- @class table

Object:add_getters{
	bounding_box = function(self)
		local m = rawget(self, "__model")
		if not m then return Aabb{point = Vector(-0.1,-0.1,-0.1), size = Vector(0.2,0.2,0.2)} end
		return m.bounding_box
	end,
	center_offset = function(self)
		local m = rawget(self, "__model")
		if not m then return Vector() end
		return m.center_offset
	end,
	id = function(self) return Los.object_get_id(self.handle) end,
	model = function(self) return rawget(self, "__model") end,
	model_name = function(self)
		local m = rawget(self, "__model")
		return m and m.name or ""
	end,
	position = function(self)
		local v = rawget(self, "__position")
		if not v then
			v = Vector()
			rawset(self, "__position", v)
		end
		Los.object_get_position(self.handle, v.handle)
		return v
	end,
	rotation = function(self)
		local v = rawget(self, "__rotation")
		if not v then
			v = Quaternion()
			rawset(self, "__rotation", v)
		end
		Los.object_get_rotation(self.handle, v.handle)
		return v
	end,
	realized = function(self) return Los.object_get_realized(self.handle) end,
	sector = function(self) return Los.object_get_sector(self.handle) end,
	static = function(self, v) return Los.object_get_static(self.handle, v) end}

Object:add_setters{
	id = function(self, v) return Los.object_set_id(self.handle, v) end,
	model = function(self, v)
		local m = v
		if type(v) == "string" then m = Model:find_or_load{file = v, mesh = Object.load_meshes} end
		rawset(self, "__model", m)
		rawset(self, "__particle", nil)
		Los.object_set_model(self.handle, m and m.handle)
	end,
	model_name = function(self, v) self.model = Model:find_or_load{file = v, mesh = Object.load_meshes} end,
	position = function(self, v) Los.object_set_position(self.handle, v.handle) end,
	rotation = function(self, v) Los.object_set_rotation(self.handle, v.handle) end,
	realized = function(self, v)
		if v then
			if __objects_realized[self] then return end
			__objects_realized[self] = true
			Los.object_set_realized(self.handle, true)
			Program:push_event{type = "object-visibility", object = self, visible = true}
		else
			if not __objects_realized[self] then return end
			__objects_realized[self] = nil
			Los.object_set_realized(self.handle, false)
			Program:push_event{type = "object-visibility", object = self, visible = false}
		end
	end,
	static = function(self, v) Los.object_set_static(self.handle, v) end}

__objects_realized = {}
