--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.serialize
-- @alias Serialize

local Class = require("system/class")

--- TODO:doc
-- @type Serialize
local Serialize = Class("Serialize")

--- Creates a new serializer.
-- @param clss Serialize class.
-- @param db Database.
-- @return Serializer.
Serialize.new = function(clss, db)
	local self = Class.new(clss)
	self.db = db
	return self
end

--- Saves everything.
-- @param clss Serialize class.
-- @param erase True to erase existing database entries first.
Serialize.save = function(clss, erase)
	if erase then Server.object_database:clear_objects() end
	Main.game.sectors:save_world(erase)
	if not erase then
		Server.object_database:update_world_decay()
		Server.object_database:clear_unused_objects()
	end
	Server.account_database:save_accounts(erase)
	Main.unlocks:save()
end

return Serialize
