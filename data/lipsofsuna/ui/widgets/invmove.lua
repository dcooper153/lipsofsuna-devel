require(Mod.path .. "invitem")

Widgets.Uiinvmove = Class(Widgets.Uiinvitem)
Widgets.Uiinvmove.class_name = "Widgets.Uiinvmove"

Widgets.Uiinvmove.new = function(clss, id, item, index, slot)
	local self = Widgets.Uiinvitem.new(clss, id, item, index, slot)
	self.hint = "$A: Place\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uiinvmove.apply = function(self)
	Network:send{packet = Packet(packets.PLAYER_MOVE_ITEM,
		"uint32", self.inventory_id,
		"uint32", Client.data.inventory.index,
		"uint32", self.index)}
	Ui:pop_state()
	Effect:play_global("uitransition1")
end
