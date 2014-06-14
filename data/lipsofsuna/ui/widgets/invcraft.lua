local Class = require("system/class")
local UiInvItem = require("ui/widgets/invitem")

local UiInvCraft = Class("UiInvCraft", UiInvItem)

UiInvCraft.new = function(clss, id, name)
	local spec = Main.specs:find_by_name("ItemSpec", name)
	local self = UiInvItem.new(clss, id, {text = name, icon = spec.icon})
	self.name = name
	self.hint = "$A: Craft\n$$B\n$$U\n$$D"
	return self
end

UiInvCraft.apply = function(self)
	if not self.enabled then return end
	Main.messaging:client_event("craft", self.inventory_id, self.name)
	Main.effect_manager:play_global("uitransition1")
end

--- Toggles whether the item is craftable.
-- @param self Widget.
-- @param value True for craftable, false for not.
UiInvCraft.set_enabled = function(self, value)
	if self.enabled == value then return end
	self.enabled = value
	self.need_repaint = true
end

return UiInvCraft
