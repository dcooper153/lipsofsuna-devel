--- Vision-based event dispatcher.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.vision.vision_manager
-- @alias VisionManager

local Class = require("system/class")
local Vision = require("system/vision")

--- Vision-based event dispatcher.
-- @type VisionManager
local VisionManager = Class("VisionManager")

--- Creates a new vision manager.
-- @param clss VisionManager class.
-- @param objects ObjectManager.
-- @return VisionManager.
VisionManager.new = function(clss, objects)
	local self = Class.new(clss)
	self.objects = objects
	return self
end

--- Emits a global event.
-- @param self VisionManager.
-- @param name Event name.
-- @param args Event arguments. Nil for an empty table.
VisionManager.global_event = function(self, name, args)
	local a = args or {}
	a.type = name
	Vision:dispatch_event(a, self.objects)
end

--- Emits an effect event for an object.
-- @param self VisionManager.
-- @param object Object.
-- @param name Effect name.
VisionManager.object_effect = function(self, object, name)
	if not name then return end
	self:object_event(object, "object-effect", {effect = name})
end

--- Emits an object event.
-- @param self VisionManager.
-- @param object Object.
-- @param name Event name.
-- @param args Event arguments. Nil for an empty table.
VisionManager.object_event = function(self, object, name, args)
	local a = args or {}
	a.id = object:get_id()
	a.object = object
	a.type = name
	Vision:dispatch_event(a, self.objects)
end

--- Emits an object event by object ID.
-- @param self VisionManager.
-- @param id Object ID.
-- @param name Event name.
-- @param args Event arguments. Nil for an empty table.
VisionManager.object_event_id = function(self, id, name, args)
	local a = args or {}
	a.id = id
	a.object = self.objects:find_by_id(id)
	a.type = name
	Vision:dispatch_event(a, self.objects)
end

--- Emits a global effect event.
-- @param self VisionManager.
-- @param point Point in world space.
-- @param name Event name.
VisionManager.world_effect = function(self, point, name)
	if not name then return end
	local args = {type = "world-effect", point = point, effect = name}
	Vision:dispatch_event(args, self.objects)
end

return VisionManager
