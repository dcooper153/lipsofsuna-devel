require "server/trading"

Dialog = Class()
Dialog.dict_id = {}
Dialog.dict_name = {}
Dialog.dict_user = {}
Dialog.flags = {}

--- Creates a new dialog.
-- @param clss Quest class.
-- @param args Arguments.<ul>
--   <li>spec: Dialog spec.</li></ul>
-- @return New dialog.
Dialog.new = function(clss, args)
	local self = Class.new(clss, args)
	self.spec = args.spec
	self.name = args.spec.name
	-- Find a free ID.
	local id = 1
	while clss.dict_id[id] do id = math.random(1, 65535) end
	self.id = id
	-- Add to dictionaries.
	clss.dict_id[self.id] = self
	clss.dict_name[self.name] = self
	return self
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
	Dialog.dict_id[self.id] = nil
	Dialog.dict_name[self.name] = nil
	if self.user then
		Dialog.dict_user[self.user] = nil
	end
	self.answer = nil
	self.object = nil
	self.user = nil
	self.routine = nil
end

--- Executes the dialog.
-- @param self Dialog.
Dialog.execute = function(self)
	-- Command handlers of the virtual machine.
	-- Handlers increment stack pointers and push and pop command arrays to the stack.
	local commands = {
		branch = function(vm, c)
			vm[1].pos = vm[1].pos + 1
			if (c.cond == nil or Dialog.flags[c.cond]) and
			   (c.cond_dead == nil or self.object.dead) and
			   (c.cond_not == nil or not Dialog.flags[c.cond_not]) then
				table.insert(vm, 1, {exe = c, off = 1, pos = 1, len = #c - 1})
			end
		end,
		["break"] = function(vm, c)
			local num = c[2] or 1
			for i = 1,num do
				if not vm[1] then break end
				table.remove(vm, 1)
			end
		end,
		choice = function(vm, c)
			local cmd = c
			local cmds = {}
			local opts = {}
			repeat
				if (cmd.cond == nil or Dialog.flags[cmd.cond]) and
				   (cmd.cond_dead == nil or self.object.dead) and
				   (cmd.cond_not == nil or not Dialog.flags[cmd.cond_not]) then
					table.insert(opts, cmd[2])
					cmds[cmd[2]] = cmd
				end
				vm[1].pos = vm[1].pos + 1
				cmd = vm[1].exe[vm[1].pos + vm[1].off]
			until not cmd or cmd[1] ~= "choice"
			if #opts then
				local sel = cmds[self:choice(opts)]
				table.insert(vm, 1, {exe = sel, off = 2, pos = 1, len = #sel - 2})
			end
		end,
		exit = function(vm, c)
			for i = #vm,1,-1 do vm[i] = nil end
		end,
		effect = function(vm, c)
			Effect:play{effect = c[2], object = self.object}
			vm[1].pos = vm[1].pos + 1
		end,
		["effect player"] = function(vm, c)
			Effect:play{effect = c[2], object = self.user}
			vm[1].pos = vm[1].pos + 1
		end,
		feat = function(vm, c)
			Feat:unlock(c)
			vm[1].pos = vm[1].pos + 1
		end,
		flag = function(vm, c)
			Dialog.flags[c[2]] = "true"
			Serialize:save_quests()
			vm[1].pos = vm[1].pos + 1
		end,
		func = function(vm, c)
			local f = c[2]
			vm[1].pos = vm[1].pos + 1
			f(self)
		end,
		["give player item"] = function(vm, c)
			vm[1].pos = vm[1].pos + 1
			local s = Itemspec:find{name = c[2]}
			if not s then return end
			local o = Item{spec = s}
			if self.user:give_item(o) then
				self.user:send("Received " .. c[2])
			else
				self.user:send("Received " .. c[2] .. " but couldn't carry it")
			end
		end,
		info = function(vm, c)
			self:line(string.format("(%s)", c[2]))
			vm[1].pos = vm[1].pos + 1
		end,
		loop = function(vm, c)
			vm[1].pos = 1
		end,
		loot = function(vm, c)
			self.object:loot(self.user)
			for i = #vm,1,-1 do vm[i] = nil end
		end,
		quest = function(vm, c)
			local q = Quest:find{name = c[2]}
			if q then q:update(c) end
			vm[1].pos = vm[1].pos + 1
		end,
		random = function(vm, c)
			local o = math.random(2, #c)
			vm[1].pos = vm[1].pos + 1
			table.insert(vm, 1, {exe = c, off = o - 1, pos = 1, len = 1})
		end,
		["remove player item"] = function(vm, c)
			vm[1].pos = vm[1].pos + 1
			local o = self.user:get_item{name = c[2]}
			if o then
				self.user:send("Lost " .. c[2])
				o:detach()
				table.insert(vm, 1, {exe = c, off = 2, pos = 1, len = 1})
			elseif #c >= 3 then
				table.insert(vm, 1, {exe = c, off = 3, pos = 1, len = 1})
			end
		end,
		["require player item"] = function(vm, c)
			vm[1].pos = vm[1].pos + 1
			local o = self.user:get_item{name = c[2]}
			if o then
				table.insert(vm, 1, {exe = c, off = 2, pos = 1, len = 1})
			elseif #c >= 3 then
				table.insert(vm, 1, {exe = c, off = 3, pos = 1, len = 1})
			end
		end,
		say = function(vm, c)
			self:line(string.format("%s: %s", c[2], c[3]))
			vm[1].pos = vm[1].pos + 1
		end,
		teleport = function(vm, c)
			self.user:teleport(c)
			vm[1].pos = vm[1].pos + 1
		end,
		trade = function(vm, c)
			Trading:start(self.user, self.object)
			for i = #vm,1,-1 do vm[i] = nil end
		end}
	-- Initialize the virtual machine stack.
	local vm = {{exe = self.spec.commands, off = 0, pos = 1, len = #self.spec.commands}}
	-- Execute commands until the stack is empty.
	while vm[1] do
		if vm[1].pos > vm[1].len then
			table.remove(vm, 1)
		else
			local cmd = vm[1].exe[vm[1].pos + vm[1].off]
			local fun = commands[cmd[1]]
			fun(vm, cmd)
		end
	end
end

--- Attempts to start a dialog.
-- @param clss Dialog class.
-- @param args Arguments.<ul>
--   <li>name: Dialog name.</li>
--   <li>object: Object whose dialog to start.</li>
--   <li>user: Object starting the dialog.</li></ul>
-- @return True if started.
Dialog.start = function(clss, args)
	-- Allow the target object and the player object both to engage
	-- into only one dialog at a time.
	if args.object.dialog then return end
	if clss.dict_user[args.user] then return end
	-- Find the dialog spec.
	local name = args.name or args.object.spec.dialog
	local spec = Dialogspec:find{name = name}
	if not spec then return end
	if spec.unique and clss.dict_name[name] then return end
	-- Create the dialog.
	local dialog = Dialog{spec = spec}
	-- Initialize the state.
	dialog.answer = nil
	dialog.object = args.object
	dialog.user = args.user
	if args.user then
		clss.dict_user[args.user] = true
	end
	-- Attach to the object.
	dialog.object.dialog = dialog
	if dialog.object.spec.type == "creature" then
		dialog.object:update_ai_state()
	end
	-- Start the coroutine.
	dialog.routine = coroutine.create(function() dialog:execute() end)
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

