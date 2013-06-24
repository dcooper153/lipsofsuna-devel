--- Keeps track of dialogs between players and NPCs.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.dialog.dialog_manager
-- @alias DialogManager

local Class = require("system/class")
local Dialog = require("core/dialog/dialog")

--- Keeps track of dialogs between players and NPCs.
-- @type DialogManager
local DialogManager = Class("DialogManager")

--- Creates a new dialog manager.
-- @param clss DialogManager class.
-- @return DialogManager.
DialogManager.new = function(clss)
	local self = Class.new(clss)
	self.dialogs_by_object = setmetatable({}, {__mode = "k"})
	return self
end

--- Cancles the dialog of the given object.
-- @param self DialogManager.
-- @param object Object whose dialog to cancel.
DialogManager.cancel = function(self, object)
	-- Find the dialog.
	local dialog = self.dialogs_by_object[object]
	if not dialog then return end
	-- Notify clients of the removal.
	dialog:emit_event(object, {})
	-- Remove from the dictionary.
	self.dialogs_by_object[object] = nil
	dialog.object = nil
	dialog.user = nil
	object.dialog = nil
end

--- Executes the dialog of the given object.
-- @param self DialogManager.
-- @param object Object whose dialog to execute.
-- @param user Player who triggered the dialog.
-- @param special String for special dialog. Nil for default.
-- @return True if executed. False otherwise.
DialogManager.execute = function(self, object, user, special)
	-- Only allow one dialog at a time.
	if self.dialogs_by_object[object] then return end
	-- Construct the dialog spec name.
	local name = object.spec.dialog
	if not name then return end
	if special then
		name = name .. " " .. special
	end
	-- Find the dialog spec.
	local spec = Dialogspec:find{name = object.spec.dialog}
	if not spec then return end
	-- Create the dialog.
	local dialog = Dialog(object, user or object, spec)
	self.dialogs_by_object[object] = dialog
	object.dialog = dialog
	-- Execute the dialog.
	if dialog:execute() then
		if object.spec.type == "actor" then
			object:update_ai_state()
		end
		return true
	end
end

--- Finds the dialog for the given object.
-- @param self DialogManager.
-- @param object Object.
-- @return Dialog if found. Nil otherwise.
DialogManager.find_by_object = function(self, object)
	return self.dialogs_by_object[object]
end

return DialogManager
