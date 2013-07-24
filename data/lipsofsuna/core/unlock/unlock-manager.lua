--- Manages unlocks.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.unlock.unlock_manager
-- @alias UnlockManager

local ActionSpec = require("core/specs/action")
local Class = require("system/class")
local ModifierSpec = require("core/specs/modifier")
local Skillspec = require("core/specs/skill")

--- Manages unlocks.
-- @type UnlockManager
local UnlockManager = Class("UnlockManager")

--- Initializes unlocks.
-- @param clss UnlockManager class.
-- @param db Database, or nil.
-- @return UnlockManager
UnlockManager.new = function(clss, db)
	local self = Class.new(clss)
	self.__messaging = false
	self.db = db
	self.unlocks = {}
	return self
end

--- Clears all unlocks.
-- @param self UnlockManager.
UnlockManager.reset = function(self)
	self.unlocks = {}
end

--- Clears all unlocks from the database.
-- @param self UnlockManager.
UnlockManager.reset_database = function(self)
	if self.db then
		self.db:query([[DROP TABLE IF EXISTS unlocks;]])
		self.db:query([[CREATE TABLE unlocks (
			type TEXT,
			name TEXT,
			PRIMARY KEY(type,name));]])
	end
end

--- Called when an unlock is added or removed.
-- @param self UnlockManager.
-- @param type Unlock type.
-- @param name Unlock name.
-- @param added True if added, false if removed.
UnlockManager.changed = function(self, type, name, added)
	if not self.__messaging then return end
	if added then
		Main.messaging:server_event_broadcast("unlocks add", type, name)
	else
		Main.messaging:server_event_broadcast("unlocks remove", type, name)
	end
end

--- Returns true if the item is unlocked.
-- @param self UnlockManager.
-- @param type Unlock type.
-- @param name Unlock name.
UnlockManager.get = function(self, type, name)
	if not self.unlocks[type] then return end
	if not self.unlocks[type][name] then return end
	return true
end

--- Gets the unlocks in a list form.
-- @param self UnlockManager.
-- @return List of unlocks.
UnlockManager.get_list = function(self)
	local res = {}
	for type,names in pairs(self.unlocks) do
		for name in pairs(names) do
			table.insert(res, {type, name})
		end
	end
	return res
end

--- Locks an entry of the given type with the given name.
-- @param self UnlockManager.
-- @param type Unlock type.
-- @param name Unlock name.
UnlockManager.lock = function(self, type, name)
	-- Remove from the unlock table.
	if not self.unlocks[type] then return end
	if not self.unlocks[type][name] then return end
	self.unlocks[type][name] = nil
	-- Call the change callback.
	self:changed(type, name, false)
	-- Update the database.
	if self.db then
		self.db:query([[DELETE FROM unlocks WHERE type=? AND name=?;]], {type, name})
	end
end

--- Unlocks an entry of the given type with the given name.
-- @param self UnlockManager.
-- @param type Unlock type.
-- @param name Unlock name.
UnlockManager.unlock = function(self, type, name)
	-- Add to the unlock table.
	if not self.unlocks[type] then
		self.unlocks[type] = {[name] = true}
	else
		if self.unlocks[type][name] then return end
		self.unlocks[type][name] = true
	end
	-- Call the change callback.
	self:changed(type, name, true)
	-- Update the database.
	if self.db then
		self.db:query([[REPLACE INTO unlocks (type,name) VALUES (?,?);]], {type, name})
	end
end

--- Unlocks all skills, actions and modifiers.
-- @param self UnlockManager.
UnlockManager.unlock_all = function(self)
	repeat until not self:unlock_random()
end

--- Unlock a random skill, actions or modifier.
-- @param self UnlockManager.
-- @return Unlock type and unlock name, or nil.
UnlockManager.unlock_random = function(self)
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
	-- Find the unlockable actions.
	-- All spell types that have a description are assumed to be used by
	-- players. Out of those, we choose ones not yet unlocked.
	for k,v in pairs(ActionSpec.dict_name) do
		if v.description and not self:get("action", k) then
			table.insert(choices, {"action", k})
		end
	end
	-- Find the unlockable modifiers.
	-- All modifiers that have a description are assumed to be used by
	-- players. Out of those, we choose ones that are not yet unlocked but
	-- have at least one of the allowed actions unlocked.
	for k,v in pairs(ModifierSpec.dict_name) do
		if v.description and not self:get("modifier", k) then
			local pass = false
			for name in pairs(v.actions) do
				if self:get("action", name) then
					pass = true
					break
				end
			end
			if pass then
				table.insert(choices, {"modifier", k})
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
-- @param self UnlockManager.
UnlockManager.load = function(self)
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
-- @param self UnlockManager.
UnlockManager.save = function(self)
	if not self.db then return end
	self.db:query([[DELETE FROM unlocks;]])
	for type,names in pairs(self.unlocks) do
		for name in pairs(names) do
			self.db:query([[INSERT INTO unlocks (type,name) VALUES (?,?);]], {type, name})
		end
	end
end

--- Sets the database used by the manager.
-- @param self UnlockManager.
-- @param value Database. Nil to disable loading and saving.
UnlockManager.set_database = function(self, value)
	self.db = value
end

--- Enables or disables messaging.
-- @param self UnlockManager.
-- @param value True to enable. False otherwise.
UnlockManager.set_messaging = function(self, value)
	self.__messaging = value
end

return UnlockManager
