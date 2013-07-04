local Class = require("system/class")
local UiInvItem = require("ui/widgets/invitem")

Widgets.Uieditoritem = Class("Uieditoritem", UiInvItem)

Widgets.Uieditoritem.new = function(clss, name)
	local spec = Itemspec:find_by_name(name)
	local self = UiInvItem.new(clss, nil, {text = name, icon = spec.icon})
	self.name = name
	self.hint = "$A: Create\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uieditoritem.apply = function(self)
	Client.editor:create_item(self.name)
	Client.effects:play_global("uitransition1")
end
