Dialog = Class()
Dialog.dict_id = {}
Dialog.dict_name = {}

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
	-- Detach from the object.
	self.object.dialog = nil
	-- Clear the state.
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
	-- Only allow one dialog at a time.
	if args.object.dialog then return end
	-- Find the dialog.
	local dialog = Dialog:find{name = args.object.species.dialog}
	if not dialog then return end
	-- Create a new instance if not unique.
	if not dialog.unique then
		dialog = dialog:copy()
	end
	-- Initialize the state.
	dialog.answer = nil
	dialog.object = args.object
	dialog.user = args.user
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
-- @param msg Message string.
-- @param opt List of accepted answers.
-- @return Answer.
Dialog.dialog = function(self, msg, opt)
	local dict = {}
	local packet = Packet(packets.DIALOG_DATA, "uint32", self.id, "string", msg)
	for k,v in ipairs(opt) do
		dict[v] = k
		packet:write("string", v)
	end
	self.user:send{packet = packet}
	while not self.answer or not dict[self.answer] do
		coroutine.yield()
	end
	local answer = self.answer
	self.answer = nil
	return answer
end

------------------------------------------------------------------------------

Protocol:add_handler{type = "DIALOG_ANSWER", func = function(event)
	local ok,id,msg = event.packet:read("uint32", "string")
	if ok then
		local dialog = Dialog.dict_id[id]
		local object = Player:find{client = event.client}
		if dialog and dialog.user == object and not dialog.answer then
			dialog.answer = msg
		end
	end
end}

Protocol:add_handler{type = "DIALOG_CLOSE", func = function(event)
	local ok,id = event.packet:read("uint32")
	if ok then
		local dialog = Dialog.dict_id[id]
		local object = Player:find{client = event.client}
		if dialog and dialog.user == object then
			dialog:close()
		end
	end
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

