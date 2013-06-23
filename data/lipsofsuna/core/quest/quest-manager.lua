--- Managers quests and quest variables.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.quest_manager
-- @alias QuestManager

local Class = require("system/class")
local Quest = require("core/quest/quest")

--- Managers quests and quest variables.
-- @type QuestManager
local QuestManager = Class("QuestManager")

--- Creates a new quest database.
-- @param clss QuestManager class.
-- @return QuestManager.
QuestManager.new = function(clss)
	local self = Class.new(clss)
	self.flags = {}
	self.quests_by_name = {}
	self.variables = {}
	return self
end

--- Resets the quests.
-- @param self QuestManager.
QuestManager.reset = function(self)
	self.quests_by_name = {}
end

--- Finds a quest by name.
-- @param self QuestManager.
-- @param name Quest name.
-- @return Quest, or nil.
QuestManager.find_quest_by_name = function(self, name)
	return self.quests_by_name[name]
end

--- Loads all quests.
-- @param self QuestManager.
QuestManager.load_quests = function(self)
end

--- Saves a quest.
-- @param self QuestManager.
-- @param quest Quest.
QuestManager.save_quest = function(self, quest)
end

--- Returns the dictionary of all quests.
-- @param self QuestManager.
-- @return Dictionary.
QuestManager.get_all_quests = function(self)
	return self.quests_by_name
end

--- Gets a dialog flag by name.
-- @param self QuestManager.
-- @param name Flag name.
-- @return Flag value if found. Nil otherwise.
QuestManager.get_dialog_flag = function(self, name)
	return self.flags[name]
end

--- Sets a dialog flag.
-- @param self QuestManager.
-- @param name Flag name.
-- @param value Flag value.
QuestManager.set_dialog_flag = function(self, name, value)
	self.flags[name] = value
end

--- Gets a dialog variable by the owner object and variable name.
-- @param self QuestManager.
-- @param object Object.
-- @param name Variable name.
-- @return Variable value if found. Nil otherwise.
QuestManager.get_dialog_variable = function(self, object, name)
	local vars = self.variables[object:get_id()]
	if not vars then return end
	return vars[name]
end

--- Sets the given dialog variable of the owner object.
-- @param self QuestManager.
-- @param object Object.
-- @param name Variable name.
-- @param value Variable value.
QuestManager.set_dialog_variable = function(self, object, name, value)
	local id = object:get_id()
	if not self.variables[id] then
		if not value then return end
		self.variables[id] = {[name] = value}
	else
		self.variables[id][name] = value
	end
end

--- Gets all dialog variables for the object.
-- @param self QuestManager.
-- @param object Object.
-- @return Dictionary of variables.
QuestManager.get_dialog_variables = function(self, object)
	return self.variables[object:get_id()] or {}
end

return QuestManager
