local EnchantState = require("crafting/ui/enchant-state")
local ModifierSpec = require("core/specs/modifier")
local UiButton = require("ui/widgets/button")
local UiLabel = require("ui/widgets/label")
local UiSelector = require("ui/widgets/selector")
local UiSpell = require("ui/widgets/spell")

local state = EnchantState()

Ui:add_state{
	state = "enchant",
	label = "Enchant",
	init = function()
		local ok,err = state:init()
		if err then return {UiLabel(err)} end
	end}

Ui:add_widget{
	state = "enchant",
	widget = function()
		if state:get_state() ~= "disenchant" then return end
		return UiButton("Disenchant", function() state:disenchant() end)
	end}

Ui:add_widget{
	state = "enchant",
	widget = function()
		if state:get_state() ~= "enchant" then return end
		local actions,index = state:get_actions()
		local widget = UiSelector("Mode", actions)
		widget:select(index)
		return widget
	end}

Ui:add_widget{
	state = "enchant",
	widget = function()
		if state:get_state() ~= "enchant" then return end
		-- Create the modifier removal buttons.
		local widgets = {}
		for k,v in ipairs(state:get_modifiers()) do
			local widget = UiSpell(v, true)
			widget:set_hint("$A: Remove\n$$B\n$$U\n$$D")
			widget.apply = function() state:remove_modifier(k) end
			table.insert(widgets, widget)
		end
		-- Create the modifier addition button
		local modifiers = state:get_modifiers_available()
		if modifiers then
			table.insert(widgets, UiSelector("Add an enchantment...", modifiers))
		end
		return widgets
	end}

Ui:add_widget{
	state = "enchant",
	widget = function()
		if not state:can_enchant() then return end
		return UiButton("Enchant", function() state:enchant() end)
	end}
