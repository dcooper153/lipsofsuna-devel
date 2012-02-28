require(Mod.path .. "label")

Widgets.Uidialogchoice = Class(Widgets.Uilabel)
Widgets.Uidialogchoice.class_name = "Widgets.Uidialogchoice"

Widgets.Uidialogchoice.new = function(clss, id, index, text)
	local self = Widgets.Uilabel.new(clss, text)
	self.dialog_id = id
	self.dialog_index = index
	self.hint = "$A: Reply\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uidialogchoice.apply = function(self)
	Network:send{packet = Packet(packets.DIALOG_ANSWER, "uint32", self.dialog_id, "string", self.text)}
end
