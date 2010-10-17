Dialog = Class()
Dialog.dialogs = {}

--- Creates a new conversation dialog.
-- @param self Dialog class.
-- @param id Unique dialog number.
-- @param msg Message string.
-- @param opt List of valid answers.
function Dialog.create(self, id, msg, opt)
	self:close(id)
	local dialog = Group{cols = 1, style = "window", margins = {5,5,5,5}}
	dialog:append_row(Button{style = "quest-label", text = msg})
	for k,v in pairs(opt) do
		dialog:append_row(Button{style = "quest-button", text = v, pressed = function()
			Network:send{packet = Packet(packets.DIALOG_ANSWER, "uint32", id, "string", v)}
			self.dialogs[id] = nil
			dialog.visible = false
		end})
	end
	self.dialogs[id] = dialog
	dialog.floating = true
end

--- Closes a conversation dialog.
-- @param self Dialog class.
-- @param id Unique dialog number.
function Dialog.close(self, id)
	local dialog = self.dialogs[id]
	if dialog then
		dialog.floating = false
	end
	self.dialogs[id] = nil
end

------------------------------------------------------------------------------

-- Creates a conversation dialog.
Protocol:add_handler{type = "DIALOG_DATA", func = function(event)
	local ok,id,msg = event.packet:read("uint32", "string")
	if ok then
		local opts = {}
		while true do
			local ok,opt = event.packet:resume("string")
			if not ok then break end
			table.insert(opts, opt)
		end
		Dialog:create(id, msg, opts)
	end
end}

-- Deletes a conversation dialog.
Protocol:add_handler{type = "DIALOG_CLOSE", func = function(event)
	local ok,id = event.packet:read("uint32")
	if ok then Dialog:close(id) end
end}
