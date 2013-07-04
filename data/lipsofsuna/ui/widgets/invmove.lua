local Class = require("system/class")
local UiInvItem = require("ui/widgets/invitem")

Widgets.Uiinvmove = Class("Uiinvmove", UiInvItem)

Widgets.Uiinvmove.new = function(clss, id, item, index, slot)
	local self = UiInvItem.new(clss, id, item, index, slot)
	self.hint = "$A: Place\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uiinvmove.apply = function(self)
	Main.messaging:client_event("move in inventory", self.inventory_id, Client.data.inventory.index, self.index)
	Ui:pop_state()
	Client.effects:play_global("uitransition1")
end
