require "server/trading"

Dialog = Class()
Dialog.dict_id = {}
Dialog.flags = {}

--- Creates a new dialog.
-- @param clss Quest class.
-- @param args Arguments.<ul>
--   <li>name: Dialog name.</li>
--   <li>object: Object controlled by the dialog.</li>
--   <li>spec: Dialog spec.</li>
--   <li>user: Object that started the dialog.</li></ul>
-- @return New dialog.
Dialog.new = function(clss, args)
	-- Find the dialog spec.
	local name = args.name or (args.spec and args.spec.name) or args.object.spec.dialog
	local spec = args.spec or Dialogspec:find{name = name}
	if not spec then return end
	-- Allocate self.
	local self = Class.new(clss, args)
	self.id = args.object.id
	self.name = name
	self.object = args.object
	self.spec = spec
	self.user = args.user
	-- Add to dictionaries.
	clss.dict_id[self.id] = self
	-- Attach to the object.
	self.object.dialog = self
	if self.object.spec.type == "creature" then
		self.object:update_ai_state()
	end
	-- Initialize the virtual machine.
	self.vm = {{exe = self.spec.commands, off = 0, pos = 1, len = #self.spec.commands}}
	return self
end

--- Answers a choice or finishes the shown message.<br/>
-- Continues a paused dialog until the next choice or message is encountered.
-- @param self Dialog.
-- @param user Object answering.
-- @param answer Answer string.
Dialog.answer = function(self, user, answer)
	if type(self.choices) == "table" then
		-- Choice.
		local sel = self.choices[answer]
		if not sel then return end
		self.user = user
		self.choices = nil
		table.insert(self.vm, 1, {exe = sel, off = 2, pos = 1, len = #sel - 2})
		self:execute()
	elseif self.choices == "info" or self.choices == "line" then
		-- Info or say.
		self.vm[1].pos = self.vm[1].pos + 1
		self.user = user
		self.choices = nil
		self:execute()
	end
end

--- Executes the dialog.<br/>
-- Continues the dialog until the next choice or message is encountered or
-- the dialog ends.
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
			-- Construct the list of choices.
			local cmd = c
			local cmds = {}
			local choices = {}
			repeat
				if (cmd.cond == nil or Dialog.flags[cmd.cond]) and
				   (cmd.cond_dead == nil or self.object.dead) and
				   (cmd.cond_not == nil or not Dialog.flags[cmd.cond_not]) then
					table.insert(choices, cmd[2])
					cmds[cmd[2]] = cmd
				end
				vm[1].pos = vm[1].pos + 1
				cmd = vm[1].exe[vm[1].pos + vm[1].off]
			until not cmd or cmd[1] ~= "choice"
			-- Publish the choices.
			local event = {type = "object-dialog", object = self.object, choices = choices}
			self.event = event
			Vision:event(event)
			-- Break until answered.
			self.choices = cmds
			self.user = nil
			return true
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
			-- Publish the info.
			self.event = {type = "object-dialog", object = self.object, message = string.format("(%s)", c[2])}
			Vision:event(self.event)
			-- Break until answered.
			self.choices = "info"
			self.user = nil
			return true
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
			-- Publish the line.
			self.event = {type = "object-dialog", object = self.object, character = c[2], message = c[3]}
			Vision:event(self.event)
			-- Break until answered.
			self.choices = "line"
			self.user = nil
			return true
		end,
		teleport = function(vm, c)
			self.user:teleport(c)
			vm[1].pos = vm[1].pos + 1
		end,
		trade = function(vm, c)
			Trading:start(self.user, self.object)
			for i = #vm,1,-1 do vm[i] = nil end
		end}
	-- Execute commands until break or end.
	local vm = self.vm
	while vm[1] do
		if vm[1].pos > vm[1].len then
			table.remove(vm, 1)
		else
			local cmd = vm[1].exe[vm[1].pos + vm[1].off]
			local fun = commands[cmd[1]]
			local brk = fun(vm, cmd)
			if brk then return end
		end
	end
	-- Reset at end.
	Vision:event{type = "object-dialog", object = self.object}
	Dialog.dict_id[self.id] = nil
	self.object.dialog = nil
	self.object = nil
	self.user = nil
end

------------------------------------------------------------------------------

Protocol:add_handler{type = "DIALOG_ANSWER", func = function(event)
	local ok,id,msg = event.packet:read("uint32", "string")
	if not ok then return end
	local dialog = Dialog.dict_id[id]
	local player = Player:find{client = event.client}
	if not dialog or not dialog.choices or not player.vision.objects[dialog.object] then return end
	dialog:answer(player, msg)
end}
