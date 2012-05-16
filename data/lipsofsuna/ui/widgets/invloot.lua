require(Mod.path .. "invitem")

Widgets.Uiinvloot = Class(Widgets.Uiinvitem)
Widgets.Uiinvloot.class_name = "Widgets.Uiinvloot"

Widgets.Uiinvloot.new = function(clss, id, item, index, slot)
	local self = Widgets.Uiinvitem.new(clss, id, item, index, slot)
	self.hint = "$A: Take\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uiinvloot.apply = function(self)
	Network:send{packet = Packet(packets.PLAYER_TAKE, "uint32", self.inventory_id, "uint32", self.index)}
	Effect:play_global("uitransition1")
end
