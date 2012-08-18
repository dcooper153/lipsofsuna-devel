require "system/class"

QuestDatabase = Class()

--- Creates a new quest database.
-- @param clss QuestDatabase class.
-- @param db Database.
-- @returns QuestDatabase.
QuestDatabase.new = function(clss, db)
	local self = Class.new(clss)
	self.db = db
	return self
end

--- Resets the quest database.
-- @param self QuestDatabase.
QuestDatabase.reset = function(self)
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

--- Loads quests from the database.
-- @param self QuestDatabase.
QuestDatabase.load_quests = function(self)
	local r = self.db:query("SELECT name,status,desc,marker FROM quests;")
	for k,v in ipairs(r) do
		local quest = Quest:find{name = v[1]}
		if quest then
			quest:update{status = v[2], text = v[3], marker = v[4]}
		end
	end
	local r = self.db:query("SELECT name,value FROM dialog_flags;")
	for k,v in ipairs(r) do
		Dialog.flags[v[1]] = v[2]
	end
end

--- Saves a quest.
-- @param self QuestDatabase.
-- @param quest Quest.
QuestDatabase.save_quest = function(self, quest)
	self.db:query("BEGIN TRANSACTION;")
	self.db:query("REPLACE INTO quests (name,status,desc,marker) VALUES (?,?,?,?);",
		{quest.name, quest.status, quest.text, quest.marker})
	self.db:query("END TRANSACTION;")
end

--- Saves all quests.
-- @param self QuestDatabase.
-- @param erase True to erase existing database entries first.
QuestDatabase.save_quests = function(self, erase)
	self.db:query("BEGIN TRANSACTION;")
	self.db:query("DELETE FROM quests;")
	self.db:query("DELETE FROM dialog_flags;")
	for k,v in pairs(Quest.dict_name) do
		self.db:query("REPLACE INTO quests (name,status,desc,marker) VALUES (?,?,?,?);",
			{k, v.status, v.text, v.marker})
	end
	for k,v in pairs(Dialog.flags) do
		self.db:query("REPLACE INTO dialog_flags (name,value) VALUES (?,?);", {k, v})
	end
	self.db:query("END TRANSACTION;")
end

--- Gets a dialog variable by the owner object and variable name.
-- @param self QuestDatabase.
-- @param object Object.
-- @param name Variable name.
-- @return Variable value, or nil.
QuestDatabase.get_dialog_variable = function(self, object, name)
	local rows = self.db:query([[SELECT value FROM dialog_variables WHERE id=? AND key=?;]], {object.id, name})
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
		self.db:query([[REPLACE INTO dialog_variables (id,key,value) VALUES (?,?,?);]], {object.id, name, value})
	else
		self.db:query([[DELETE FROM dialog_variables WHERE id=? AND key=?;]], {object.id, name})
	end
end

--- Gets all dialog variables for the object.
-- @param self QuestDatabase.
-- @param object Object.
-- @return Dictionary of variables.
QuestDatabase.get_dialog_variables = function(self, object)
	local res = {}
	local rows = self.db:query([[SELECT key,value FROM dialog_variables WHERE id=?;]], {object.id})
	if rows then
		for k,v in ipairs(rows) do
			res[v[1]] = v[2]
		end
	end
	return res
end
