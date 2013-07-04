local Class = require("system/class")
local UiInvItem = require("ui/widgets/invitem")

Widgets.Uiinvstore = Class("Uiinvstore", UiInvItem)

Widgets.Uiinvstore.new = function(clss, id, item, index, slot)
	local self = UiInvItem.new(clss, id, item, index, slot)
	self.hint = "$A: Store\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uiinvstore.apply = function(self)
	Main.messaging:client_event("store to container", self.inventory_id, self.index)
	Client.effects:play_global("uitransition1")
end
