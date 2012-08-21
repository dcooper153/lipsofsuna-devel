local Class = require("system/class")
require(Mod.path .. "label")

Widgets.Uihelplink = Class("Uihelplink", Widgets.Uilabel)

Widgets.Uihelplink.new = function(clss, text, link)
	local self = Widgets.Uilabel.new(clss, text)
	self.link = link
	self.hint = "$A: Show topic\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uihelplink.apply = function(self)
	Client.data.help.page = self.link
	Ui:set_state("help")
	Client.effects:play_global("uitransition1")
end
