--- Database-backed quest manager.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.quest_database
-- @alias QuestDatabase

local Class = require("system/class")
local Quest = require("core/quest/quest")
local QuestManager = require("core/quest/quest-manager")

--- Database-backed quest manager.
-- @type QuestDatabase
local QuestDatabase = Class("QuestDatabase", QuestManager)

--- Creates a new quest database.
-- @param clss QuestDatabase class.
-- @param db Database.
-- @return QuestDatabase.
QuestDatabase.new = function(clss, db)
	local self = QuestManager.new(clss)
	self.db = db
	return self
end

--- Resets the quests.
-- @param self QuestDatabase.
QuestDatabase.reset = function(self)
	QuestManager.reset(self)
	self.db:query([[DROP TABLE IF EXISTS dialog_flags;]])
	self.db:query([[DROP TABLE IF EXISTS dialog_variables;]])
	self.db:query([[DROP TABLE IF EXISTS quests;]])
	self.db:query([[CREATE TABLE dialog_flags (
		name TEXT PRIMARY KEY,
		value TEXT);]])
	self.db:query([[CREATE TABLE dialog_variables (
		id INTEGER,
		key TEXT,
		value TEXT,
		PRIMARY KEY(id,key));]])
	self.db:query([[CREATE TABLE quests (
		name TEXT PRIMARY KEY,
		status TEXT,
		desc TEXT,
		marker TEXT);]])
end

--- Loads all quests.
-- @param self QuestDatabase.
QuestDatabase.load_quests = function(self)
	local r = self.db:query([[SELECT name,status,desc,marker FROM quests;]])
	for k,v in ipairs(r) do
		local quest = Quest(self, unpack(v))
		self.quests_by_name[quest.name] = quest
	end
end

--- Saves a quest.
-- @param self QuestDatabase.
-- @param quest Quest.
QuestDatabase.save_quest = function(self, quest)
	self.db:query([[REPLACE INTO quests (name,status,desc,marker) VALUES (?,?,?,?);]],
		{quest.name, quest.status, quest.text, quest.marker})
end

--- Gets a dialog flag by name.
-- @param self QuestDatabase.
-- @param name Flag name.
-- @return Flag value, or nil.
QuestDatabase.get_dialog_flag = function(self, name)
	local rows = self.db:query([[SELECT value FROM dialog_flags WHERE name=?;]], {name})
	if not rows then return end
	for k,v in ipairs(rows) do
		return v[1]
	end
end

--- Sets a dialog flag.
-- @param self QuestDatabase.
-- @param name Flag name.
-- @param value Flag value.
QuestDatabase.set_dialog_flag = function(self, name, value)
	if value then
		self.db:query([[REPLACE INTO dialog_flags (name,value) VALUES (?,?);]], {name, value})
	else
		self.db:query([[DELETE FROM dialog_flags WHERE name=?;]], {name})
	end
end

--- Gets a dialog variable by the owner object and variable name.
-- @param self QuestDatabase.
-- @param object Object.
-- @param name Variable name.
-- @return Variable value, or nil.
QuestDatabase.get_dialog_variable = function(self, object, name)
	local rows = self.db:query([[SELECT value FROM dialog_variables WHERE id=? AND key=?;]], {object:get_id(), name})
	if not rows then return end
	for k,v in ipairs(rows) do
		return v[1]
	end
end

--- Sets the given dialog variable of the owner object.
-- @param self QuestDatabase.
-- @param object Object.
-- @param name Variable name.
-- @param value Variable value.
QuestDatabase.set_dialog_variable = function(self, object, name, value)
	if value then
		self.db:query([[REPLACE INTO dialog_variables (id,key,value) VALUES (?,?,?);]], {object:get_id(), name, value})
	else
		self.db:query([[DELETE FROM dialog_variables WHERE id=? AND key=?;]], {object:get_id(), name})
	end
end

--- Gets all dialog variables for the object.
-- @param self QuestDatabase.
-- @param object Object.
-- @return Dictionary of variables.
QuestDatabase.get_dialog_variables = function(self, object)
	local res = {}
	local rows = self.db:query([[SELECT key,value FROM dialog_variables WHERE id=?;]], {object:get_id()})
	if rows then
		for k,v in ipairs(rows) do
			res[v[1]] = v[2]
		end
	end
	return res
end

return QuestDatabase
