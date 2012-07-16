require "system/class"
require "system/eventhandler"

if not Los.program_load_extension("vision") then
	error("loading extension `vision' failed")
end

------------------------------------------------------------------------------

Vision = Class()
Vision.class_name = "Vision"
Vision.dict = {}

--- Creates a vision manager.
-- @param clss Vision class.
-- @param args Arguments.
-- @return New tile.
Vision.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.vision_new()
	self.id = args and args.id or 0
	self.events = {}
	self.objects = setmetatable({}, {__mode = "k"})
	if args then
		for k,v in pairs(args) do self[k] = v end
	end
	return self
end

--- Emits a custom vision event.
-- @param clss Vision class.
-- @param args Event arguments.
Vision.event = function(clss, args)
	if args.id then args.object = Object:find{id = args.id} end
	if args.vision then
		-- Vision events.
		local vision = clss.dict[args.vision]
		if not vision then return end
		if args.type == "vision-object-shown" then
			args.type = "object-shown"
			args.object = Object:find{id = args.object}
			if not args.object then return end
			if vision.objects[args.object] then return end
			vision.objects[args.object] = true
		elseif args.type == "vision-object-hidden" then
			args.type = "object-hidden"
			args.object = Object:find{id = args.object}
			if not args.object then return end
			if not vision.objects[args.object] then return end
			vision.objects[args.object] = nil
		elseif args.type == "vision-voxel-block-changed" then
			args.type = "voxel-block-changed"
		else
			return
		end
		table.insert(vision.events, args)
	elseif args.object then
		-- Object events.
		for k,v in pairs(clss.dict) do
			if v.objects[args.object] then
				table.insert(v.events, args)
			end
		end
	elseif args.point then
		-- Point events.
		for k,v in pairs(clss.dict) do
			if (v.position - args.point).length <= v.radius then
				table.insert(v.events, args)
			end
		end
	end
end

--- Clears the vision.
-- @param self Vision.
Vision.clear = function(self)
	Los.vision_clear(self.handle)
	self.objects = {}
	self.events = {}
end

--- Updates the vision.
-- @param self Vision.
Vision.update = function(self)
	-- Pop the events.
	local e = self.events
	self.events = {}
	-- Call the callback with the events.
	if self.callback then
		for k,v in ipairs(e) do
			self.callback(v)
		end
	end
	-- Check for more events.
	Los.vision_update(self.handle)
	return e
end

Vision:add_getters{
	cone_angle = function(self) return Los.vision_get_cone_angle(self.handle) end,
	cone_factor = function(self) return Los.vision_get_cone_factor(self.handle) end,
	direction = function(self)
		return Class.new(Vector, {handle = Los.vision_get_direction(self.handle)})
	end,
	enabled = function(self) return Vision.dict[self.id] ~= nil end,
	id = function(self) return Los.vision_get_id(self.handle) end,
	position = function(self)
		return Class.new(Vector, {handle = Los.vision_get_position(self.handle)})
	end,
	radius = function(self) return Los.vision_get_radius(self.handle) end,
	threshold = function(self) return Los.vision_get_threshold(self.handle) end}

Vision:add_setters{
	cone_angle = function(self, v) Los.vision_set_cone_angle(self.handle, v) end,
	cone_factor = function(self, v) Los.vision_set_cone_factor(self.handle, v) end,
	direction = function(self, v) Los.vision_set_direction(self.handle, v.handle) end,
	enabled = function(self, v) Vision.dict[self.id] = self or nil end,
	id = function(self, v) Los.vision_set_id(self.handle, v) end,
	position = function(self, v) Los.vision_set_position(self.handle, v.handle) end,
	radius = function(self, v) Los.vision_set_radius(self.handle, v) end,
	threshold = function(self, v) Los.vision_set_threshold(self.handle, v) end}

------------------------------------------------------------------------------
-- Event handler hooks.

Eventhandler{type = "object-motion", func = function(self, event)
	Vision:event{type = "object-moved", object = event.object}
end}

Eventhandler{type = "vision-object-shown", func = function(self, event)
	Vision:event(event)
end}

Eventhandler{type = "vision-object-hidden", func = function(self, event)
	Vision:event(event)
end}

Eventhandler{type = "vision-voxel-block-changed", func = function(self, event)
	Vision:event(event)
end}
