local Class = require("system/class")
local IconSpec = require("core/specs/icon")
local UiButton = require("ui/widgets/button")
local UiMenu = require("ui/widgets/menu")
local UiInvItem = Class("UiInvItem", UiMenu)
local UiTransition = require("ui/widgets/transition")

UiInvItem.new = function(clss, id, item, index, slot)
	local self = UiMenu.new(clss)
	self.enabled = true
	self.inventory_id = id
	self.item = item
	self.index = index
	self.slot = slot
	self.hint = item and "$A: Manipulate\n$$B\n$$U\n$$D" or "$$B\n$$U\n$$D"
	return self
end

UiInvItem.apply = function(self)
	if not self.item then return end
	Client.data.inventory.id = self.inventory_id
	Client.data.inventory.index = self.index
	Client.data.inventory.count = self.item.count or 1
	-- Create the selector widgets.
	local id = self.inventory_id
	local index = self.index
	local count = self.item.count or 1
	local parent = self
	local creators =
	{
		function()
			-- Get the active container.
			if not id then return end
			local object = Main.objects:find_by_id(id)
			if not object then return end
			-- Get the active item.
			local item = object.inventory:get_object_by_index(index)
			if not item then return end
			-- Get the equipment slot.
			local slot = item.spec.equipment_slot
			if not slot then return end
			-- Don't show the widget if the item is already equipped.
			if object.inventory:get_slot_by_index(index) then return end
			-- Create the widget.
			return UiButton("Equip in " .. slot, function()
				Main.messaging:client_event("equip from inventory", index, slot)
				parent:set_menu_opened(false)
			end)
		end,
		function()
			-- Get the active container.
			local object = Main.objects:find_by_id(id)
			if not object then return end
			-- Don't show the widget if the item isn't equipped.
			if not object.inventory:get_slot_by_index(index) then return end
			-- Create the widget.
			return UiButton("Unequip", function()
				Main.messaging:client_event("unequip", index)
				parent:set_menu_opened(false)
			end)
		end,
		function()
			-- Get the active container.
			if not id then return end
			local object = Main.objects:find_by_id(id)
			if not object then return end
			-- Get the active item.
			local item = object.inventory:get_object_by_index(index)
			if not item then return end
			-- Add a widget for each special usage.
			local widgets = {}
			for k,v in pairs(item.spec:get_use_actions()) do
				local name = v.label or v.name
				local action = v.name
				table.insert(widgets, UiButton(name, function()
					Main.messaging:client_event("use in inventory", id, index, action)
					parent:set_menu_opened(false)
				end))
			end
			return widgets
		end,
		function()
			return UiButton("Drop", function()
				Main.messaging:client_event("drop from inventory", index, count)
				parent:set_menu_opened(false)
			end)
		end,
		function()
			-- Don't show the widget if the count isn't at least two.
			if count < 2 then return end
			-- Create the widget.
			return UiTransition("Drop stack", "inventory/drop",
				function() parent:set_menu_opened(false) end)
		end,
		function()
			-- Don't show the widget if the count isn't at least two.
			if count < 2 then return end
			-- Create the widget.
			return UiTransition("Split stack", "inventory/split",
				function() parent:set_menu_opened(false) end)
		end,
		function()
			return UiTransition("Move", "inventory/move",
				function() parent:set_menu_opened(false) end)
		end
	}
	widgets = {}
	for k,v in ipairs(creators) do
		local widget = v()
		if widget then
			if not widget.class then
				for k1,v1 in ipairs(widget) do
					table.insert(widgets, v1)
				end
			else
				table.insert(widgets, widget)
			end
		end
	end
	self:set_menu_widgets(widgets)
	self:set_menu_opened(true)
end

UiInvItem.right = function(self)
	self:apply()
end

UiInvItem.rebuild_size = function(self)
	-- Get the base size.
	local size = UiMenu.rebuild_size(self)
	-- Resize to fit the description.
	local name = self:get_pretty_name()
	if name then
		local w,h = Program:measure_text(Theme.text_font_1, name, size.x - 40)
		if h then size.y = math.max(size.y, h + 10) end
	end
	return size
end

UiInvItem.rebuild_canvas = function(self)
	local w = self.size.x
	local h = self.size.y
	-- Add the base.
	UiMenu.rebuild_canvas(self)
	if not self.item then return end
	-- Add the icon.
	local icon = IconSpec:find_by_name(self.item.icon)
	if icon then
		self:canvas_image{
			dest_position = {5,5},
			dest_size = {icon.size[1],icon.size[2]},
			color = {1,1,1,1},
			source_image = icon.image,
			source_position = icon.offset,
			source_tiling = {0,icon.size[1],0,0,icon.size[2],0}}
	end
	-- Add the name.
	local name = self:get_pretty_name()
	if name then
		self:canvas_text{
			dest_position = {40,5},
			dest_size = {w-40,h},
			text = name,
			text_alignment = {0,0},
			text_color = self.enabled and Theme.text_color_1 or Theme.text_color_2,
			text_font = Theme.text_font_2}
	end
	-- Add the equip text.
	if self.slot then
		self:canvas_text{
			dest_position = {5,5},
			dest_size = {w-10,h},
			text = "E",
			text_alignment = {1,0},
			text_color = Theme.text_color_1,
			text_font = Theme.text_font_2}
	end
	-- Add the count.
	local count = self:get_pretty_count()
	if count then
		self:canvas_text{
			dest_position = {5,5},
			dest_size = {w-10-1.5*Theme.text_height_2,h},
			text = count,
			text_alignment = {1,0},
			text_color = Theme.text_color_1,
			text_font = Theme.text_font_2}
	end
end

UiInvItem.get_pretty_count = function(self)
	if not self.item then return end
	if not self.item.count or self.item.count <= 1 then return end
	return tostring(self.item.count)
end

UiInvItem.get_pretty_name = function(self)
	if not self.item then return end
	if not self.item.text then return end
	local subs = function(a,b) return string.upper(a) .. b end
	return string.gsub(self.item.text, "(.)(.*)", subs)
end

return UiInvItem
