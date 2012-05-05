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

--- Unlocks a random skill, spell type or spell effect.
-- @param self Unlocks class.
-- @return Unlock type and unlock name, or nil.
Unlocks.unlock_random = function(self)
	local choices = {}
	-- Find the unlockable skills.
	-- All skill types that have a description are assumed to be used by
	-- players. Such a skill can be unlocked if it hasn't been unlocked
	-- already but all its requirements have been unlocked.
	for k,v in pairs(Skillspec.dict_name) do
		if v.description and not self:get("skill", k) then
			local deps = v:find_direct_requirements()
			local pass = true
			for name in pairs(deps) do
				if not self:get("skill", name) then
					pass = false
					break
				end
			end
			if pass then
				table.insert(choices, {"skill", k})
			end
		end
	end
	-- Find the unlockable spell types.
	-- All spell types that have a description are assumed to be used by
	-- players. Out of those, we choose ones not yet unlocked.
	for k,v in pairs(Feattypespec.dict_name) do
		if v.description and not self:get("spell type", k) then
			table.insert(choices, {"spell type", k})
		end
	end
	-- Find the unlockable spell effects.
	-- All spell effects that have a description are assumed to be used by
	-- players. Out of those, we choose ones that are not yet unlocked but
	-- have at least one of the allowed spell types unlocked.
	for k,v in pairs(Feateffectspec.dict_name) do
		if v.description and not self:get("spell effect", k) then
			local pass = false
			for index,name in pairs(v.animations) do
				if self:get("spell type", name) then
					pass = true
					break
				end
			end
			if pass then
				table.insert(choices, {"spell effect", k})
			end
		end
	end
	-- Unlock a random item.
	if #choices == 0 then return end
	local c = choices[math.random(1, #choices)]
	self:unlock(c[1], c[2])
	return c[1], c[2]
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
