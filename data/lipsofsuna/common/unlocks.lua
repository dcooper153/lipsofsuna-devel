Unlocks = Class()
Unlocks.class_name = "Unlocks"

--- Initializes unlocks.
-- @param self Unlocks class.
-- @param db Database, or nil.
Unlocks.init = function(self, db)
	self.db = db
	self.unlocks = {}
end

--- Called when an unlock is added or removed.
-- @param self Unlocks class.
-- @param type Unlock type.
-- @param name Unlock name.
Unlocks.changed = function(self, type, name)
end

--- Returne true if the item is unlocked.
-- @param self Unlocks class.
-- @param type Unlock type.
-- @param name Unlock name.
Unlocks.get = function(self, type, name)
	if not self.unlocks[type] then return end
	if not self.unlocks[type][name] then return end
	return true
end

--- Locks an item.
-- @param self Unlocks class.
-- @param type Unlock type.
-- @param name Unlock name.
Unlocks.lock = function(self, type, name)
	-- Remove from the unlock table.
	if not self.unlocks[type] then return end
	if not self.unlocks[type][name] then return end
	self.unlocks[type][name] = nil
	-- Call the change callback.
	self:changed(type, name)
	-- Update the database.
	if self.db then
		self.db:query([[DELETE FROM unlocks WHERE type=? AND name=?;]], {type, name})
	end
end

--- Unlocks an item.
-- @param self Unlocks class.
-- @param type Unlock type.
-- @param name Unlock name.
Unlocks.unlock = function(self, type, name)
	-- Add to the unlock table.
	if not self.unlocks[type] then
		self.unlocks[type] = {[name] = true}
	else
		if self.unlocks[type][name] then return end
		self.unlocks[type][name] = true
	end
	-- Call the change callback.
	self:changed(type, name)
	-- Update the database.
	if self.db then
		self.db:query([[REPLACE INTO unlocks (type,name) VALUES (?,?);]], {type, name})
	end
end

--- Reads the unlocks from a database.
-- @param self Unlocks class.
Unlocks.read_db = function(self)
	self.unlocks = {}
	if not self.db then return end
	local rows = self.db:query([[SELECT type,name FROM unlocks;]])
	if not rows then return unlocks end
	for k,v in ipairs(rows) do
		if not self.unlocks[v[1]] then
			self.unlocks[v[1]] = {[v[2]] = true}
		else
			self.unlocks[v[1]][v[2]] = true
		end
	end
end

--- Writes the unlocks to a database.
-- @param self Unlocks class.
Unlocks.write_db = function(self)
	if not self.db then return end
	self.db:query([[DELETE FROM unlocks;]])
	for type,names in pairs(self.unlocks) do
		for name in pairs(names) do
			self.db:query([[INSERT INTO unlocks (type,name) VALUES (?,?);]], {type, name})
		end
	end
end

--- Writes the unlocks to a packet.
-- @param self Unlocks class.
-- @param packet Packet.
Unlocks.write_packet = function(self, packet)
	for type,names in pairs(self.unlocks) do
		for name in pairs(names) do
			packet:write("string", type, "string", name)
		end
	end
end
