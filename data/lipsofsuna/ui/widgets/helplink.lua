require(Mod.path .. "label")

Widgets.Uihelplink = Class(Widgets.Uilabel)
Widgets.Uihelplink.class_name = "Widgets.Uihelplink"

Widgets.Uihelplink.new = function(clss, text, link)
	local self = Widgets.Uilabel.new(clss, text)
	self.link = link
	self.hint = "$A: Show topic\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uihelplink.apply = function(self)
	Client.data.help.page = self.link
	Ui.state = "help"
end
