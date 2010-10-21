Dialog = Class(Group)
Dialog.active = nil

--- Creates a new conversation dialog.
-- @param clss Dialog class.
-- @param id Unique dialog number.
-- @param msg Message string.
-- @param opt List of choices.
Dialog.new = function(clss, id, msg, opt)
	-- Create the widget.
	local self = Group.new(clss, {cols = 1, id = id, margins = {5,5,5,5}})
	if msg then
		self:append_row(Button{style = "quest-label", text = msg, pressed = function()
			Network:send{packet = Packet(packets.DIALOG_ANSWER, "uint32", id, "string", "")}
		end})
	else
		for k,v in pairs(opt) do
			self:append_row(Button{style = "quest-button", text = v, pressed = function()
				Network:send{packet = Packet(packets.DIALOG_ANSWER, "uint32", id, "string", v)}
			end})
		end
	end
	-- Make it active.
	clss.active = self
	Gui.menus:open{level = 1, widget = self}
end

--- Closes a conversation dialog.
-- @param self Dialog.
-- @param silent Don't notify the server.
Dialog.close = function(self, silent)
	if Dialog.active == self then
		Dialog.active = nil
		Gui.menus:close()
		if not silent then
			Network:send{packet = Packet(packets.DIALOG_CLOSE, "uint32", self.id)}
		end
	end
end

Dialog.find = function(clss, id)
	return clss.dict_id[id]
end

------------------------------------------------------------------------------

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
	if Dialog.active then
		Dialog.active:close(true)
	end
	Dialog(id, nil, opts)
end}
Protocol:add_handler{type = "DIALOG_LINE", func = function(event)
	local ok,id,msg = event.packet:read("uint32", "string")
	if not ok then return end
	if Dialog.active then
		Dialog.active:close(true)
	end
	Dialog(id, msg, nil)
end}

-- Deletes a conversation dialog.
Protocol:add_handler{type = "DIALOG_CLOSE", func = function(event)
	local ok,id = event.packet:read("uint32")
	if not ok then return end
	local dialog = Dialog:find(id)
	if not dialog then return end
	dialog:close(true)
end}
