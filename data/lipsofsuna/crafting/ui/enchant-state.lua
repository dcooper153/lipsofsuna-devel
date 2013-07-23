--- Enchantment state.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module crafting.crafting_utils
-- @alias EnchantState

local Class = require("system/class")
local ModifierSpec = require("core/specs/modifier")

--- Enchantment state.
-- @type EnchantState
local EnchantState = Class("EnchantState")

--- Creates new enchantment state.
-- @param clss EnchantState class.
-- @return EnchantState.
EnchantState.new = function(clss)
	local self = Class.new(clss)
	self.__action = nil
	self.__state = nil
	self.__modifiers = {}
	return self
end

--- Adds a modifier.
-- @param self EnchantState.
-- @param value ModifierSpec.
EnchantState.add_modifier = function(self, value)
	table.insert(self.__modifiers, value)
	Ui:restart_state()
end

--- Checks if everything is ready for enchanting.
-- @param self EnchantState.
-- @return True if can enchant. False otherwise.
EnchantState.can_enchant = function(self)
	if self.__state ~= "enchant" then return end
	if not self.__action then return end
	if not self.__modifiers[1] then return end
	return true
end

--- Disenchants the currently wielded weapon.
-- @param self EnchantState.
EnchantState.disenchant = function(self)
	Main.messaging:client_event("disenchant")
	Ui:pop_state()
end

--- Enchants the currently wielded weapon.
-- @param self EnchantState.
EnchantState.enchant = function(self)
	local modifiers = {}
	for k,v in pairs(self.__modifiers) do
		modifiers[v.name] = v.spell_strength
	end
	Main.messaging:client_event("enchant", self.__action.name, modifiers)
	Ui:pop_state()
end

--- Initializes the enchantment state.
-- @param self EnchantState.
-- @return True if succeeded. False and error message otherwise.
EnchantState.init = function(self)
	-- Reset the state.
	self.__state = nil
	-- Get the player object.
	local object = Client.player_object
	if not object then
		return false,"Player not found"
	end
	-- Get the weapon.
	local weapon = object:get_weapon()
	if not weapon then
		return false,"Wield a weapon first to enchant it."
	end
	-- Determine the state.
	if Main.crafting_utils:is_item_enchanted(weapon) then
		self.__state = "disenchant"
	else
		self.__state = "enchant"
	end
	-- Select the default action.
	if not self.__action then
		for k,v in ipairs(Actionspec:find_by_category("enchantment")) do
			self.__action = v
			break
		end
	end
	return true
end

--- Removes the modifier.
-- @param self EnchantState.
-- @param index Index into the modifier list.
EnchantState.remove_modifier = function(self, index)
	table.remove(self.__modifiers, index)
	Ui:restart_state()
end

--- Sets the current action.
-- @param self EnchantState.
-- @param value Actionspec.
EnchantState.set_action = function(self, value)
	self.__action = value
	self.__modifiers = {}
	Ui:restart_state()
end

--- Gets the actions for the current weapon.
-- @param self EnchantState.
-- @return List of actions, the index of the active action.
EnchantState.get_actions = function(self)
	-- Validate the state.
	if self.__state ~= "enchant" then return end
	-- Get the player object.
	local object = Client.player_object
	if not object then return end
	-- Get the weapon.
	local weapon = object:get_weapon()
	if not weapon then return end
	-- Create the action selector.
	local actions = {}
	local index = 1
	for k,v in ipairs(Actionspec:find_by_category("enchantment")) do
		if Client.data.unlocks:get("action", v.name) then
			table.insert(actions, {v.name, function() self:set_action(v) end})
			if self.__action == v then
				index = k
			end
		end
	end
	return actions,index
end

--- Gets the modifiers currently added.
-- @param self EnchantState.
-- @return List of modifiers. Nil if not applicable.
EnchantState.get_modifiers = function(self)
	-- Validate the state.
	if self.__state ~= "enchant" then return end
	if not self.__action then return end
	-- Get the modifiers.
	return self.__modifiers
end

--- Gets the modifiers that can be added.
-- @param self EnchantState.
-- @return List of modifiers. Nil if no more can be added.
EnchantState.get_modifiers_available = function(self)
	-- Validate the state.
	if self.__state ~= "enchant" then return end
	if not self.__action then return end
	if #self.__modifiers >= 3 then return end
	-- Get the modifiers.
	local modifiers = {}
	for k,v in ipairs(ModifierSpec:find_by_category("spell")) do
		if Client.data.unlocks:get("modifier", v.name) then
			if v.actions[self.__action.name] then
				table.insert(modifiers, {v.name, function() self:add_modifier(v) end})
			end
		end
	end
	return modifiers
end

--- Gets the current enchantment state.
-- @param self EnchantState.
-- @return String.
EnchantState.get_state = function(self)
	return self.__state
end

return EnchantState
