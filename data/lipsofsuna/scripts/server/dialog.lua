Dialog = Class()
Dialog.dict_id = {}
Dialog.dict_name = {}
Dialog.dict_user = {}

--- Creates a new dialog.
-- @param clss Quest class.
-- @param args Arguments.<ul>
--   <li>die: Death dialog function.</li>
--   <li>main: Normal dialog function.</li>
--   <li>name: Unique name.</li>
--   <li>unique: Only one instance of the dialog is allowed to run at once.</li></ul>
-- @return New dialog.
Dialog.new = function(clss, args)
	local self = Class.new(clss, args)
	-- Find a free ID.
	local id = 1
	while clss.dict_id[id] do id = math.random(1, 65535) end
	self.id = id
	-- Add to the dictionaries.
	clss.dict_id[self.id] = self
	clss.dict_name[self.name] = self
	return self
end

--- Creates a copy of the dialog.<br/>
-- @param self Dialog.
-- @return Dialog.
Dialog.copy = function(self)
	local copy = Class.new(Dialog)
	-- Find a free ID.
	local id = 1
	while Dialog.dict_id[id] do id = math.random(1, 65535) end
	copy.id = id
	-- Add to the ID dictionary only.
	Dialog.dict_id[copy.id] = copy
	-- Copy other attributes.
	copy.main = self.main
	copy.name = self.name
	copy.unique = self.unique
	return copy
end

Dialog.close = function(self)
	-- Notify the user. The user might have forcefully closed the dialog
	-- or logged out, in which case we don't need to send anything.
	if self.user then
		local packet = Packet(packets.DIALOG_CLOSE, "uint32", self.id)
		self.user:send{packet = packet}
	end
	-- Detach from the object.
	self.object.dialog = nil
	-- Clear the state.
	if self.user then
		Dialog.dict_user[self.user] = nil
	end
	self.answer = nil
	self.object = nil
	self.user = nil
	self.routine = nil
	-- Remove completely if an instance.
	if not self.unique then
		Dialog.dict_id[self.id] = nil
	end
end

--- Finds a dialog.
-- @param clss Dialog class.
-- @param args Arguments.<ul>
--   <li>id: Dialog ID.</li>
--   <li>name: Dialog name.</li></ul>
-- @return Dialog or nil.
Dialog.find = function(clss, args)
	if args.id then return clss.dict_id[args.id] end
	if args.name then return clss.dict_name[args.name] end
end

--- Attempts to start a dialog.
-- @param clss Dialog class.
-- @param args Arguments.<ul>
--   <li>user: Object starting the dialog.</li>
--   <li>object: Object whose dialog to start.</li>
--   <li>type: Dialog type. ("use"/"die")</ul>
-- @return True if started.
Dialog.start = function(clss, args)
	-- Allow the target object and the player object both to engage
	-- into only one dialog at a time.
	if args.object.dialog then return end
	if clss.dict_user[args.user] then return end
	-- Find the dialog.
	local dialog = Dialog:find{name = args.object.spec.dialog}
	if not dialog then return end
	-- Create a new instance if not unique.
	if not dialog.unique then
		dialog = dialog:copy()
	end
	-- Initialize the state.
	dialog.answer = nil
	dialog.object = args.object
	dialog.user = args.user
	if args.user then
		clss.dict_user[args.user] = true
	end
	if args.type == "die" then
		dialog.routine = coroutine.create(dialog.die or function() end)
	else
		dialog.routine = coroutine.create(dialog.main or function() end)
	end
	-- Attach to the object.
	dialog.object.dialog = dialog
	dialog.object:update_ai_state()
	-- Start the coroutine.
	local r,e = coroutine.resume(dialog.routine, dialog)
	if r == false then
		print("Error in dialog `" .. dialog.name .. "': " .. e)
		dialog:close()
		return false
	end
	if coroutine.status(dialog.routine) == "dead" then
		dialog:close()
	end
	return true
end

--- Shows a conversation dialog and waits for an answer.<br/>
-- Sends a conversation packet to the user and puts the dialog thread to
-- sleep until a reply is received.
-- @param self Dialog.
-- @param args List of accepted answers.
-- @return Answer.
Dialog.choice = function(self, args)
	-- Send the choices to the client.
	local dict = {}
	if self.user then
		local packet = Packet(packets.DIALOG_CHOICE, "uint32", self.id)
		for k,v in ipairs(args) do
			dict[v] = k
			packet:write("string", v)
		end
		self.user:send{packet = packet}
	end
	-- Wait for a reply.
	while self.user and not self.answer do
		coroutine.yield()
	end
	-- Pick the default answer if the client lost control, for example due to
	-- being disconnected. If no default is explicitly specified, the last
	-- choice in the list is used.
	if not self.answer or not dict[self.answer] then
		self.answer = args[args.default or #args]
	end
	-- Return the answer.
	local answer = self.answer
	self.answer = nil
	return answer
end

--- Shows a conversation dialog and waits for a page flip.<br/>
-- Sends a conversation packet to the user and puts the dialog thread to
-- sleep until a reply is received.
-- @param self Dialog.
-- @param msg Message string.
Dialog.line = function(self, msg)
	if self.user then
		local packet = Packet(packets.DIALOG_LINE, "uint32", self.id, "string", msg)
		self.user:send{packet = packet}
	end
	while self.user and not self.answer do
		coroutine.yield()
	end
	self.answer = nil
end

------------------------------------------------------------------------------

Protocol:add_handler{type = "DIALOG_ANSWER", func = function(event)
	local ok,id,msg = event.packet:read("uint32", "string")
	if not ok then return end
	local dialog = Dialog.dict_id[id]
	local object = Player:find{client = event.client}
	if dialog and dialog.user == object and not dialog.answer then
		dialog.answer = msg
	end
end}

Protocol:add_handler{type = "DIALOG_CLOSE", func = function(event)
	local ok,id = event.packet:read("uint32")
	if not ok then return end
	-- Find the dialog and make sure the user owns it.
	local dialog = Dialog.dict_id[id]
	local object = Player:find{client = event.client}
	if not dialog or dialog.user ~= object then return end
	-- Forcefully close the dialog. This causes it to automatically
	-- execute with default choices until it reaches the end.
	Dialog.dict_user[dialog.user] = nil
	dialog.user = nil
end}

Eventhandler{type = "tick", func = function(self, args)
	for key,dialog in pairs(Dialog.dict_id) do
		if dialog.routine then
			local r,e = coroutine.resume(dialog.routine, args.secs)
			if r == false then
				print("Error in dialog `" .. dialog.name .. "': " .. e)
			end
			if coroutine.status(dialog.routine) == "dead" then
				dialog:close()
			end
		end
	end
end}

