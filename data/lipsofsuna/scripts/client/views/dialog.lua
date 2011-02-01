Views.Dialog = Class(Widget)
Views.Dialog.mode = "dialog"

--- Creates a new conversation dialog view.
-- @param clss Dialog class.
-- @return Dialog view.
Views.Dialog.new = function(clss, id, msg, opt)
	local self = Widget.new(clss, {cols = 1, id = id, margins = {5,5,5,5}})
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
	self.rows = 0
	if msg then
		-- Line.
		self:append_row(Widgets.DialogLabel{text = msg, pressed = function()
			Network:send{packet = Packet(packets.DIALOG_ANSWER, "uint32", id, "string", "")}
		end})
	else
		-- Choice.
		for k,v in pairs(opt) do
			self:append_row(Widgets.DialogChoice{text = v, pressed = function()
				Network:send{packet = Packet(packets.DIALOG_ANSWER, "uint32", id, "string", v)}
			end})
		end
	end
	Gui:set_mode("dialog")
end

------------------------------------------------------------------------------

Views.Dialog.inst = Views.Dialog()

-- Creates a conversation dialog.
Protocol:add_handler{type = "DIALOG_CHOICE", func = function(event)
	local ok,id = event.packet:read("uint32")
	if not ok then return end
	local opts = {}
	while true do
		local ok,opt = event.packet:resume("string")
		if not ok then break end
		table.insert(opts, opt)
	end
	Views.Dialog.inst:show(id, nil, opts)
end}
Protocol:add_handler{type = "DIALOG_LINE", func = function(event)
	local ok,id,msg = event.packet:read("uint32", "string")
	if not ok then return end
	Views.Dialog.inst:show(id, msg, nil)
end}

-- Deletes a conversation dialog.
Protocol:add_handler{type = "DIALOG_CLOSE", func = function(event)
	local ok,id = event.packet:read("uint32")
	if not ok then return end
	if Views.Dialog.inst.id ~= id then return end
	Views.Dialog.inst.id = nil
	Gui:set_mode("game")
end}
