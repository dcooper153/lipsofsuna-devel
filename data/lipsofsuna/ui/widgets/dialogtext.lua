require(Mod.path .. "label")

Widgets.Uidialogtext = Class(Widgets.Uilabel)
Widgets.Uidialogtext.class_name = "Widgets.Uidialogtext"

Widgets.Uidialogtext.new = function(clss, id, index, text)
	local self = Widgets.Uilabel.new(clss, text)
	self.dialog_id = id
	self.dialog_index = index
	self.hint = "$A: Next line\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uidialogtext.apply = function(self)
	Network:send{packet = Packet(packets.DIALOG_ANSWER, "uint32", self.dialog_id, "string", "")}
end
