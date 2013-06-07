--- Map marker.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module lipsofsuna.core.marker
-- @alias Marker

local Class = require("system/class")

--- Map marker.
-- @type Marker
local Marker = Class("Marker")
Marker.dict_name = {}
Marker.dict_discoverable = {}

--- Finds a map marker.
-- @param clss Marker class.
-- @param args Arguments.<ul>
--   <li>name: Marker name.</li></ul>
-- @return Marker or nil.
Marker.find = function(clss, args)
	return clss.dict_name[args.name]
end

--- Creates a new map marker.
-- @param clss Marker class.
-- @param args Arguments.<ul>
--   <li>discoverable: True to allow the marker to be automatically discovered.</li>
--   <li>name: Marker name.</li>
--   <li>target: ID of the target object.</li></ul>
-- @return Marker.
Marker.new = function(clss, args)
	local self = Class.new(clss)
	for k,v in pairs(args) do self[k] = v end
	clss.dict_name[args.name] = self
	if self.discoverable then
		clss.dict_discoverable[args.name] = self
	end
	return self
end

--- Resets the map marker system.
-- @param clss Marker class.
Marker.reset = function(clss)
	for k,v in pairs(clss.dict_name) do
		v:disable()
	end
	clss.dict_name = {}
end

--- Disables updates for the map marker.
-- @param self Marker.
Marker.disable = function(self)
	if self.timer then
		self.timer:disable()
		self.timer = nil
	end
end

Marker.unlock = function(self)
	if self.unlocked then return end
	self.unlocked = true
	for k,v in pairs(Server.players_by_client) do
		Game.messaging:server_event("create marker", k, self.name, self.position)
	end
	Server.serialize:save_marker(self)
end

return Marker
