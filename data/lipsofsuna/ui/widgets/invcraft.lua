require(Mod.path .. "invitem")

Widgets.Uiinvcraft = Class(Widgets.Uiinvitem)
Widgets.Uiinvcraft.class_name = "Widgets.Uiinvcraft"

Widgets.Uiinvcraft.new = function(clss, id, name)
	local spec = Itemspec:find{name = name}
	local self = Widgets.Uiinvitem.new(clss, id, {text = name, icon = spec.icon})
	self.name = name
	self.hint = "$A: Craft\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uiinvcraft.apply = function(self)
	Network:send{packet = Packet(packets.CRAFTING, "uint32", self.inventory_id, "string", self.name)}
	Effect:play_global("uitransition1")
end
