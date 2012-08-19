require(Mod.path .. "invitem")

Widgets.Uiinvmove = Class(Widgets.Uiinvitem)
Widgets.Uiinvmove.class_name = "Widgets.Uiinvmove"

Widgets.Uiinvmove.new = function(clss, id, item, index, slot)
	local self = Widgets.Uiinvitem.new(clss, id, item, index, slot)
	self.hint = "$A: Place\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uiinvmove.apply = function(self)
	Game.messaging:client_event("move in inventory", self.inventory_id, Client.data.inventory.index, self.index)
	Ui:pop_state()
	Client.effects:play_global("uitransition1")
end
