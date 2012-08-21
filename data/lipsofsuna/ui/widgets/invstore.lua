local Class = require("system/class")
require(Mod.path .. "invitem")

Widgets.Uiinvstore = Class("Uiinvstore", Widgets.Uiinvitem)

Widgets.Uiinvstore.new = function(clss, id, item, index, slot)
	local self = Widgets.Uiinvitem.new(clss, id, item, index, slot)
	self.hint = "$A: Store\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uiinvstore.apply = function(self)
	Game.messaging:client_event("store to container", self.inventory_id, self.index)
	Client.effects:play_global("uitransition1")
end
