local Class = require("system/class")
local UiInvItem = require("ui/widgets/invitem")

Widgets.Uiinvcraft = Class("Uiinvcraft", UiInvItem)

Widgets.Uiinvcraft.new = function(clss, id, name)
	local spec = Itemspec:find_by_name(name)
	local self = UiInvItem.new(clss, id, {text = name, icon = spec.icon})
	self.name = name
	self.hint = "$A: Craft\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uiinvcraft.apply = function(self)
	if not self.enabled then return end
	Main.messaging:client_event("craft", self.inventory_id, self.name)
	Client.effects:play_global("uitransition1")
end

--- Toggles whether the item is craftable.
-- @param self Widget.
-- @param value True for craftable, false for not.
Widgets.Uiinvcraft.set_enabled = function(self, value)
	if self.enabled == value then return end
	self.enabled = value
	self.need_repaint = true
end
