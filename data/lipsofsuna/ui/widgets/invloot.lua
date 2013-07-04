local Class = require("system/class")
local UiInvItem = require("ui/widgets/invitem")

Widgets.Uiinvloot = Class("Uiinvloot", UiInvItem)

Widgets.Uiinvloot.new = function(clss, id, item, index, slot)
	local self = UiInvItem.new(clss, id, item, index, slot)
	self.hint = "$A: Take\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uiinvloot.apply = function(self)
	Main.messaging:client_event("take from inventory", self.inventory_id, self.index)
	Client.effects:play_global("uitransition1")
end
