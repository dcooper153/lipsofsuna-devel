require(Mod.path .. "widget")

Widgets.Uiinvitem = Class(Widgets.Uiwidget)
Widgets.Uiinvitem.class_name = "Widgets.Uiinvitem"

Widgets.Uiinvitem.new = function(clss, id, item, index, slot)
	local self = Widgets.Uiwidget.new(clss)
	self.enabled = true
	self.inventory_id = id
	self.item = item
	self.index = index
	self.slot = slot
	self.hint = item and "$A: Manipulate\n$$B\n$$U\n$$D" or "$$B\n$$U\n$$D"
	return self
end

Widgets.Uiinvitem.apply = function(self)
	if not self.item then return end
	Client.data.inventory.id = self.inventory_id
	Client.data.inventory.index = self.index
	Client.data.inventory.count = self.item.count or 1
	Ui:push_state("inventory/item")
	Client.effects:play_global("uitransition1")
end

Widgets.Uiinvitem.rebuild_size = function(self)
	-- Get the base size.
	local size = Widgets.Uiwidget.rebuild_size(self)
	-- Resize to fit the description.
	local name = self.pretty_name
	if name then
		local w,h = Program:measure_text(Theme.text_font_1, name, size.x - 40)
		if h then size.y = math.max(size.y, h + 10) end
	end
	return size
end

Widgets.Uiinvitem.rebuild_canvas = function(self)
	local w = self.size.x
	local h = self.size.y
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	if not self.item then return end
	-- Add the icon.
	local icon = Iconspec:find{name = self.item.icon}
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
	local name = self.pretty_name
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
	local count = self.pretty_count
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

Widgets.Uiinvitem:add_getters{
	pretty_count = function(self)
		if not self.item then return end
		if not self.item.count or self.item.count <= 1 then return end
		return tostring(self.item.count)
	end,
	pretty_name = function(self)
		if not self.item then return end
		if not self.item.text then return end
		local subs = function(a,b) return string.upper(a) .. b end
		return string.gsub(self.item.text, "(.)(.*)", subs)
	end}
