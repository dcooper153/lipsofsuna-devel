--- Manages shortcuts in the client side.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.shortcut.shortcut_manager
-- @alias ShortcutManager

local Class = require("system/class")
local Ui = require("ui/ui")

--- Manages shortcuts in the client side.
-- @type ShortcutManager
local ShortcutManager = Class("ShortcutManager")

--- Creates a new shortcut manager.
-- @param clss ShortcutManager class.
-- @return ShortcutManager.
ShortcutManager.new = function(clss)
	local self = Class.new(clss)
	self.__shortcuts = {}
	return self
end

--- Either records or activates a shortcut depending on the UI state.
-- @param self ShortcutManager.
-- @param index Shortcut index.
ShortcutManager.activate = function(self, index)
	-- Get the player object.
	local player = Client.player_object
	if not player then return end
	local id = player:get_id()
	-- Record if an inventory item was active.
	local w = Ui:get_focused_widget()
	if w and w.class_name == "UiInvItem" then
		if w.inventory_id == id and w.index then
			Client:append_log(string.format("Shortcut #%d set to use item #%d.", index, w.index))
			self:record_shortcut(index, w.index, "auto")
			return
		end
	end
	-- Invoke otherwise.
	self:invoke_shortcut(index)
end

--- Clears all the shortcuts.
-- @param self ShortcutManager.
ShortcutManager.clear_all_shortcuts = function(self)
	self.__shortcuts = {}
end

--- Clears a shortcut.
-- @param self ShortcutManager.
-- @param index Shortcut index.
ShortcutManager.clear_shortcut = function(self, index)
	self.__shortcuts[index] = nil
	Main.messaging:client_event("shortcut", index)
end

--- Loads the shortcuts from data obtained from the server.
-- @param self ShortcutManager.
-- @param shortcuts Table of shortcuts.
ShortcutManager.load_shortcuts = function(self, shortcuts)
	self.__shortcuts = {}
	for k,v in pairs(shortcuts) do
		self.__shortcuts[k] = {v[1], v[2]}
	end
end

--- Invokes a shortcut.
-- @param self ShortcutManager.
-- @param index Shortcut index.
ShortcutManager.invoke_shortcut = function(self, index)
	-- Get the player object.
	local player = Client.player_object
	if not player then return end
	local id = player:get_id()
	-- Get the shortcut.
	local s = self.__shortcuts[index]
	if not s then
		Client:append_log(string.format("Shortcut #%d is not bound to any inventory item.", index))
		return
	end
	-- Get the item.
	local item = player.inventory:get_object_by_index(s[1])
	if not item then
		Client:append_log(string.format("Shortcut #%d is bound to an empty inventory index #%d.", index, s[1]))
		return
	end
	-- Perform equip/unequip actions.
	if s[2] == "auto" or s[2] == "equip" or s[2] == "unequip" then
		local slot = item.spec.equipment_slot
		if slot then
			if player.inventory:get_slot_by_index(s[1]) then
				Main.messaging:client_event("unequip", s[1])
			else
				Main.messaging:client_event("equip from inventory", s[1], slot)
			end
			return
		end
	end
	-- Perform the default item specific action.
	if s[2] == "auto" or s[2] == "use" then
		-- FIXME: The order of the actions is not guaranteed to be sensible
		-- so this potentially breaks for items that have multiple actions.
		for k,v in ipairs(item.spec:get_use_actions()) do
			Main.messaging:client_event("use in inventory", id, s[1], v.name)
			return
		end
	end
	-- Warn about unapplicable actions.
	Client:append_log(string.format("Shortcut #%d cannot be applied to the item in inventory index #%d.", index, s[1]))
end

--- Records a shortcut.
-- @param self ShortcutManager.
-- @param index Shortcut index.
-- @param item Inventory index.
-- @param action ActionSpec name.
ShortcutManager.record_shortcut = function(self, index, item, action)
	self.__shortcuts[index] = {item, action}
	Main.messaging:client_event("shortcut", index, item, action)
end

--- Updates the shortcuts after an inventory item move.
-- @param self ShortcutManager.
-- @param src Source inventory index.
-- @param dst Destination inventory index.
ShortcutManager.remap_inventory_index = function(self, src, dst)
	for k,v in pairs(self.__shortcuts) do
		if v[1] == src then
			v[1] = dst
		elseif v[1] == dst then
			v[1] = src
		end
	end
end

return ShortcutManager
