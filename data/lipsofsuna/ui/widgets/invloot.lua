local Class = require("system/class")
local UiInvItem = require("ui/widgets/invitem")

local UiInvLoot = Class("Uiinvloot", UiInvItem)

UiInvLoot.new = function(clss, id, item, index, slot)
	local self = UiInvItem.new(clss, id, item, index, slot)
	self.hint = "$A: Take\n$$B\n$$U\n$$D"
	return self
end

UiInvLoot.apply = function(self)
	Main.messaging:client_event("take from inventory", self.inventory_id, self.index)
	Client.effects:play_global("uitransition1")
end

return UiInvLoot
