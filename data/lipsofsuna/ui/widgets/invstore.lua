require(Mod.path .. "invitem")

Widgets.Uiinvstore = Class(Widgets.Uiinvitem)
Widgets.Uiinvstore.class_name = "Widgets.Uiinvstore"

Widgets.Uiinvstore.new = function(clss, id, item, index, slot)
	local self = Widgets.Uiinvitem.new(clss, id, item, index, slot)
	self.hint = "$A: Store\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uiinvstore.apply = function(self)
	Network:send{packet = Packet(packets.PLAYER_STORE, "uint32", self.inventory_id, "uint32", self.index)}
end
