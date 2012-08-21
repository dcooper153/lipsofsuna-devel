local Class = require("system/class")
local Quest = require(Mod.path .. "quest")

local QuestDatabase = Class("QuestDatabase")

--- Creates a new quest database.
-- @param clss QuestDatabase class.
-- @param db Database.
-- @returns QuestDatabase.
QuestDatabase.new = function(clss, db)
	local self = Class.new(clss)
	self.db = db
	self.quests_by_name = {}
	return self
end

--- Resets the quest database.
-- @param self QuestDatabase.
QuestDatabase.reset = function(self)
	self.quests_by_name = {}
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

--- Finds a quest by name.
-- @param self QuestDatabase.
-- @param name Quest name.
-- @return Quest, or nil.
QuestDatabase.find_quest_by_name = function(self, name)
	return self.quests_by_name[name]
end

--- Loads all quests from the database.
-- @param self QuestDatabase.
QuestDatabase.load_quests = function(self)
	local r = self.db:query([[SELECT name,status,desc,marker FROM quests;]])
	for k,v in ipairs(r) do
		local quest = Quest(unpack(v))
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

--- Returns the dictionary of all quests in the database.
-- @param self QuestDatabase.
-- @return Dictionary.
QuestDatabase.get_all_quests = function(self)
	return self.quests_by_name
end

return QuestDatabase
