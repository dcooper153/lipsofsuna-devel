local Class = require("system/class")
local Dialog = require(Mod.path .. "dialog")

local DialogManager = Class("DialogManager")

DialogManager.new = function(clss)
	local self = Class.new(clss)
	self.dialogs_by_object = setmetatable({}, {__mode = "k"})
	return self
end

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

DialogManager.find_by_object = function(self, object)
	return self.dialogs_by_object[object]
end

return DialogManager
