require(Mod.path .. "invitem")

Widgets.Uieditoritem = Class(Widgets.Uiinvitem)
Widgets.Uieditoritem.class_name = "Widgets.Uieditoritem"

Widgets.Uieditoritem.new = function(clss, name)
	local spec = Itemspec:find{name = name}
	local self = Widgets.Uiinvitem.new(clss, nil, {text = name, icon = spec.icon})
	self.name = name
	self.hint = "$A: Create\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uieditoritem.apply = function(self)
	Client.editor:create_item(self.name)
end
