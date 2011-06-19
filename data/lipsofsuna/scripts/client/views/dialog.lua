Views.Dialog = Class(Widget)
Views.Dialog.mode = "dialog"

--- Creates a new conversation dialog view.
-- @param clss Dialog class.
-- @return Dialog view.
Views.Dialog.new = function(clss, id, msg, opt)
	local self = Widget.new(clss, {cols = 1, rows = 2, id = id, margins = {5,5,5,5}})
	self.title = Widgets.Frame{style = "title", text = ""}
	self.frame = Widgets.Frame{style = "paper", cols = 1}
	self:set_child{col = 1, row = 1, widget = self.title}
	self:set_child{col = 1, row = 2, widget = self.frame}
	return self
end

--- Closes the dialog view.
-- @param self Dialog view.
Views.Dialog.back = function(self)
	if self.id then
		Network:send{packet = Packet(packets.DIALOG_CLOSE, "uint32", self.id)}
		self.id = nil
	end
	Gui:set_mode("game")
end

--- Closes the dialog view.
-- @param self Dialog view.
Views.Dialog.close = function(self)
	if self.id then
		Network:send{packet = Packet(packets.DIALOG_CLOSE, "uint32", self.id)}
		self.id = nil
	end
end

-- @param id Unique dialog number.
-- @param msg Message string.
-- @param opt List of choices.
Views.Dialog.show = function(self, id, msg, opt)
	self.id = id
	self.frame.rows = 0
	if msg then
		-- Line.
		self.title.label.text = "Listen"
		self.frame:append_row(Widgets.DialogLabel{text = msg, pressed = function()
			Network:send{packet = Packet(packets.DIALOG_ANSWER, "uint32", id, "string", "")}
		end})
	else
		-- Choice.
		self.title.label.text = "Say"
		for k,v in pairs(opt) do
			self.frame:append_row(Widgets.DialogChoice{text = v, pressed = function()
				Network:send{packet = Packet(packets.DIALOG_ANSWER, "uint32", id, "string", v)}
			end})
		end
	end
	Gui:set_mode("dialog")
end

------------------------------------------------------------------------------

Views.Dialog.inst = Views.Dialog()
