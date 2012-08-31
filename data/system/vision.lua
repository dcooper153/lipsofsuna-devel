--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.vision
-- @alias Vision

local Class = require("system/class")

if not Los.program_load_extension("vision") then
	error("loading extension `vision' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type Vision
local Vision = Class("Vision")
Vision.dict = {}

--- Creates a vision manager.
-- @param clss Vision class.
-- @param id Unique ID.
-- @param objects Object manager.
-- @return New vision.
Vision.new = function(clss, id, objects)
	local self = Class.new(clss)
	self.handle = Los.vision_new()
	self:set_id(id)
	self.events = {}
	self.objects = setmetatable({}, {__mode = "k"})
	self.object_manager = objects
	return self
end

--- Dispatches an event to appropriate visions.
-- @param clss Vision class.
-- @param event Event table.
-- @param objects Object manager.
Vision.dispatch_event = function(clss, event, objects)
	if event.type == "vision-object-shown" then
		-- Object entered vision.
		local vision = Vision.dict[event.vision]
		if not vision then return end
		vision:push_event(event)
	elseif event.type == "vision-object-hidden" then
		-- Object left vision.
		local vision = Vision.dict[event.vision]
		if not vision then return end
		vision:push_event(event)
	elseif event.type == "vision-voxel-block-changed" then
		-- Terrain changes.
		local vision = Vision.dict[event.vision]
		if not vision then return end
		vision:push_event(event)
	elseif event.object or event.id then
		-- Object events.
		if not event.object then
			local object = objects:find_by_id(event.id)
			if not object then return end
			event.object = object
		end
		for k,v in pairs(clss.dict) do
			if v.objects[event.object] then
				v:push_event(event)
			end
		end
	elseif event.point then
		-- Point events.
		for k,v in pairs(clss.dict) do
			if (v:get_position() - event.point).length <= v:get_radius() then
				v:push_event(event)
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

--- Pushes an event to the event queue of the vision.
-- @param self Vision.
-- @param event Event.
Vision.push_event = function(self, event)
	if event.type == "vision-object-shown" then
		event.type = "object-shown"
		event.object = self.object_manager:find_by_id(event.object)
		if not event.object then return end
		if self.objects[event.object] then return end
		self.objects[event.object] = true
		table.insert(self.events, event)
	elseif event.type == "vision-object-hidden" then
		event.type = "object-hidden"
		event.object = self.object_manager:find_by_id(event.object)
		if not event.object then return end
		if not self.objects[event.object] then return end
		self.objects[event.object] = nil
		table.insert(self.events, event)
	elseif event.type == "vision-voxel-block-changed" then
		event.type = "voxel-block-changed"
		table.insert(self.events, event)
	else
		if event.id then
			event.object = self.object_manager:find_by_id(event.id)
			if not event.object then return end
		end
		table.insert(self.events, event)
	end
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

Vision.get_cone_angle = function(self) return Los.vision_get_cone_angle(self.handle) end
Vision.get_cone_factor = function(self) return Los.vision_get_cone_factor(self.handle) end
Vision.get_direction = function(self)
	return Vector:new_from_handle(Los.vision_get_direction(self.handle))
end
Vision.get_enabled = function(self) return Vision.dict[selfget_id()] ~= nil end
Vision.get_id = function(self) return Los.vision_get_id(self.handle) end
Vision.get_position = function(self)
	return Vector:new_from_handle(Los.vision_get_position(self.handle))
end
Vision.get_radius = function(self) return Los.vision_get_radius(self.handle) end
Vision.get_threshold = function(self) return Los.vision_get_threshold(self.handle) end

Vision.set_cone_angle = function(self, v) Los.vision_set_cone_angle(self.handle, v) end
Vision.set_cone_factor = function(self, v) Los.vision_set_cone_factor(self.handle, v) end
Vision.set_direction = function(self, v) Los.vision_set_direction(self.handle, v.handle) end
Vision.set_enabled = function(self, v) Vision.dict[self:get_id()] = self or nil end
Vision.set_id = function(self, v) Los.vision_set_id(self.handle, v) end
Vision.set_position = function(self, v) Los.vision_set_position(self.handle, v.handle) end
Vision.set_radius = function(self, v) Los.vision_set_radius(self.handle, v) end
Vision.set_threshold = function(self, v) Los.vision_set_threshold(self.handle, v) end

return Vision
