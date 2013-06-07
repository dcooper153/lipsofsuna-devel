--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.dialog
-- @alias Dialog

local Actor = require("core/objects/actor")
local Class = require("system/class")
local Item = require("core/objects/item")
local Marker = require("core/marker")
local Obstacle = require("core/objects/obstacle")
local Trading = require(Mod.path .. "trading")

--- TODO:doc
-- @type Dialog
local Dialog = Class("Dialog")

--- Creates a new dialog.
-- @param clss Quest class.
-- @param object Object controlled by the dialog.
-- @param user Object that started the dialog.
-- @param spec Dialog spec.
-- @return New dialog.
Dialog.new = function(clss, object, user, spec)
	-- Allocate self.
	local self = Class.new(clss)
	self.id = object:get_id()
	self.object = object
	self.spec = spec
	self.user = user
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
		Server.events:notify_action("dialog", user)
	elseif self.choices == "info" or self.choices == "line" then
		-- Info or say.
		self.vm[1].pos = self.vm[1].pos + 1
		self.user = user
		self.choices = nil
		self:execute()
		Server.events:notify_action("dialog", user)
	end
end

--- Creates a dialog for a random quest.
-- @param self Dialog.
-- @param name Quest name.
-- @param difficulty Difficulty level. ("easy"/"hard")
-- @return Dialog branch.
Dialog.create_random_quest_branch = function(self, name, difficulty)
	-- Check for an existing random quest.
	-- If one is found, the quest creation pass is skipped. The dialog tree will
	-- be built from the existing actor variables in that case.
	local npc_name = self.object.spec.name
	local npc_marker = self.object.spec.marker
	local var_name = string.gsub(string.lower(name), "[^A-Za-z]", "_")
	local var_type = self.object:get_dialog_variable(var_name .. "_type")
	local var_item = self.object:get_dialog_variable(var_name .. "_item")
	local var_actor = self.object:get_dialog_variable(var_name .. "_actor")
	local var_excuse = self.object:get_dialog_variable(var_name .. "_excuse")
	-- Quest creation functions.
	-- These implement quest initialization for each random quest type. They
	-- select and create the necessary actors and set their variables.
	local random_quests = {
		-- Find an item.
		-- The player is asked to find a specific item type and bring it to the
		-- actor. Any item of the requested type is accepted. The item type is
		-- stored to the actor variables of the NPC.
		function()
			-- Set the quest type.
			var_type = "find item"
			-- Choose the wanted item.
			local spec
			if difficulty == "hard" then
				spec = Itemspec:random()
			else
				spec = Itemspec:random{category = "material"}
			end
			var_item = spec.name
			-- Choose a random excuse.
			local excuses = {
				"I promised to bring one as a souvenir for my family.",
				"I could make big money with one.",
				"It's for my local sewing club.",
				"No questions asked, alright?",
				"I heard that they are fashionable these days.",
				"I heard that they bring luck.",
				"I'd like to play with one just like I did as a kid.",
				"This particular type of item has sentimental value to me.",
				"I need it as a replacement for the one my dog ate.",
				"My mom discarded the old one so I need a new one.",
				"It just feels so good to use one."}
			var_excuse = excuses[math.random(1, #excuses)]
			-- Set the dialog variables.
			self.object:set_dialog_variable(var_name .. "_init", nil)
			self.object:set_dialog_variable(var_name .. "_type", var_type)
			self.object:set_dialog_variable(var_name .. "_item", var_item)
			self.object:set_dialog_variable(var_name .. "_excuse", var_excuse)
		end,
		-- Kill an actor.
		-- The player is asked to kill a randomly generated unique actor. The
		-- mark randomly chosen and placed in the overworld. The name of the
		-- mark is stored to the actor variables of the NPC. The quest and NPC
		-- names are stored to the actor variables of the mark so that its
		-- death dialog can update the quest.
		function()
			-- Set the quest type.
			var_type = "kill actor"
			-- Get the list of possible target actors.
			local list = Actorspec:find{category = "scapegoat"}
			if not list then return end
			-- Randomize the order of target actors.
			local actors = {}
			for k,v in pairs(list) do
				table.insert(actors, v)
			end
			for i = 1,#actors do
				local j = math.random(1,#actors)
				actors[i],actors[j] = actors[j],actors[i]
			end
			-- Create the target actor.
			local actor
			for k,spec in ipairs(actors) do
				if not Server.quest_database:get_dialog_flag("scapegoat_alive_" .. spec.name) then
					var_actor = spec.name
					actor = Actor{
						spec = spec,
						position = Utils:find_random_overworld_point(),
						random = true,
						realized = true}
					break
				end
			end
			-- Choose a random excuse.
			local excuses = {
				"All is fair in love and war.",
				"Trust me, this is for the best of everyone.",
				"No one touches my cucumber that way.",
				"Should have thought twice before teasing me in the kindergarten.",
				"That pest knows too much.",
				"This is what happens to those who criticize my ideas.",
				"What is better than picking the mark by random?",
				"Go now, my friend. Kill, kill! Muahahaha!"}
			var_excuse = excuses[math.random(1, #excuses)]
			-- Reserve the actor for this quest.
			npc_marker = actor.spec.marker
			Server.quest_database:set_dialog_flag("scapegoat_alive_" .. actor.spec.name, "true")
			-- Set the dialog variables.
			self.object:set_dialog_variable(var_name .. "_init", nil)
			self.object:set_dialog_variable(var_name .. "_type", var_type)
			self.object:set_dialog_variable(var_name .. "_actor", var_actor)
			self.object:set_dialog_variable(var_name .. "_excuse", var_excuse)
			actor:set_dialog_variable(var_name .. "_mark_actor", npc_name)
			actor:set_dialog_variable(var_name .. "_mark_marker", self.object.spec.marker)
			actor:set_dialog_variable(var_name .. "_mark_quest", name)
		end}
	if not var_type then
		local func = random_quests[math.random(1, #random_quests)]
		func()
	end
	-- Dialog creation functions.
	-- These build the dialog trees for each random quest type.
	local random_dialogs = {
		-- Find an item.
		["find item"] = function()
			return {"branch",
				{"branch", check = {{"!var", var_name .. "_init"}},
					{"quest", name, status = "active", marker = npc_marker, text = string.format("%s has asked us to find a %s.", npc_name, var_item)},
					{"var", var_name .. "_init"}
				},
				{"say", npc_name, "Could you perhaps find me a " .. var_item .. "?"},
				{"say", npc_name, var_excuse},
				{"branch",
					{"choice", "Leave it to me."},
					{"choice", "Here is your " .. var_item .. ".",
						{"remove player item", var_item,
							{"branch",
								{"quest", name, status = "completed", marker = npc_marker, text = "The requested item has been delivered."},
								{"var clear", var_name .. "_init"},
								{"var clear", var_name .. "_type"},
								{"var clear", var_name .. "_item"},
								{"var clear", var_name .. "_excuse"},
								{"say", npc_name, "Thank you!"},
								{"unlock reward"}
							},
							{"branch",
								{"say", npc_name, "I wanted a " .. var_item .. ", but you don't have it."}
							}
						}
					}
				}
			}
			end,
		-- Kill an actor.
		["kill actor"] = function()
			return {"branch",
				{"branch", check = {{"!var", var_name .. "_init"}},
					{"quest", name, status = "active", marker = npc_marker, text = string.format("%s has asked us to kill %s.", npc_name, var_actor)},
					{"var", var_name .. "_init"}
				},
				{"say", npc_name, string.format("I need you to go kill %s.", var_actor)},
				{"say", npc_name, var_excuse},
				{"branch",
					{"choice", "Leave it to me."},
					{"choice", string.format("%s has been killed.", var_actor), check = {{"flag", var_name .. "_mark_killed"}},
						{"quest", name, status = "completed", marker = npc_marker, text = string.format("%s has been informed of %s being dead.", npc_name, var_actor)},
						{"var clear", var_name .. "_init"},
						{"var clear", var_name .. "_type"},
						{"var clear", var_name .. "_actor"},
						{"var clear", var_name .. "_excuse"},
						{"flag clear", var_name .. "_mark_killed"},
						{"say", npc_name, "Thank you!"},
						{"unlock reward"}
					}
				}
			}
			end}
	if var_type then
		local func = random_dialogs[var_type]
		if func then return func() end
	end
end

--- Emits a dialog change event.<br>
--
-- The event is stored to the dialog and emitted as a vision event so that
-- current and future observers can see it. In case of static objects, a
-- global vision event is emitted to players only.
--
-- @param self Dialog.
-- @param object Object.
-- @param event Dialog event.
Dialog.emit_event = function(self, object, event)
	if self.object.spec.type ~= "static" then
		Server:object_event(object, "object-dialog", event)
	else
		event.type = "object-dialog"
		event.id = object:get_id()
		event.object = object
		for k,v in pairs(Server.players_by_client) do
			v:vision_cb(event)
		end
	end
end

--- Executes the dialog.<br/>
-- Continues the dialog until the next choice or message is encountered or
-- the dialog ends.
-- @param self Dialog.
-- @return True if the dialog is in progress, false if it ended.
Dialog.execute = function(self)
	-- Utility functions.
	local check_cond = function(c)
		-- Backward compatibility.
		if c.cond and not Server.quest_database:get_dialog_flag(c.cond) then return end
		if c.cond_dead and not self.object.dead then return end
		if c.cond_not and Server.quest_database:get_dialog_flag(c.cond_not) then return end
		-- New condition string.
		if not c.check then return true end
		for _,cond in ipairs(c.check) do
			local type,name = cond[1],cond[2]
			if type == "dead" then
				if not self.object.dead then return end
			elseif type == "!dead" then
				if self.object.dead then return end
			elseif type == "flag" then
				if not Server.quest_database:get_dialog_flag(name) then return end
			elseif type == "!flag" then
				if Server.quest_database:get_dialog_flag(name) then return end
			elseif type == "quest active" then
				local quest = Server.quest_database:find_quest_by_name(name)
				if not quest then return end
				if quest.status ~= "active" then return end
			elseif type == "quest not active" then
				local quest = Server.quest_database:find_quest_by_name(name)
				if not quest then return end
				if quest.status == "active" then return end
			elseif type == "quest completed" then
				local quest = Server.quest_database:find_quest_by_name(name)
				if not quest then return end
				if quest.status ~= "completed" then return end
			elseif type == "quest not completed" then
				local quest = Server.quest_database:find_quest_by_name(name)
				if not quest then return end
				if quest.status == "completed" then return end
			elseif type == "var" then
				if not self.object:get_dialog_variable(name) then return end
			elseif type == "!var" then
				if self.object:get_dialog_variable(name) then return end
			end
		end
		return true
	end
	local select_spawn_position = function(c)
		if c.position_absolute then return c.position_absolute end
		local pos = self.object:get_position()
		if c.position_marker then
			local m = Marker:find{name = c.position_marker}
			if m then pos = m.position end
		end
		if c.position_relative then
			pos = pos + c.position_relative
		end
		local min = c.distance_min or 0
		local max = c.distance_max or 0
		if min < max then
			local x = min + (min-max)*math.random()
			local y = min + (min-max)*math.random()
			pos = pos + Vector(x,y)
		end
		return pos
	end
	-- Command handlers of the virtual machine.
	-- Handlers increment stack pointers and push and pop command arrays to the stack.
	local commands = {
		["branch"] = function(vm, c)
			vm[1].pos = vm[1].pos + 1
			if check_cond(c) then
				table.insert(vm, 1, {exe = c, off = 1, pos = 1, len = #c - 1})
			end
		end,
		["branch generate"] = function(vm, c)
			vm[1].pos = vm[1].pos + 1
			if check_cond(c) then
				local func = c[2]
				local branch = func(self)
				if branch then
					table.insert(vm, 1, {exe = branch, off = 1, pos = 1, len = #branch - 1})
				end
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
				if check_cond(cmd) then
					table.insert(choices, cmd[2])
					cmds[cmd[2]] = cmd
				end
				vm[1].pos = vm[1].pos + 1
				cmd = vm[1].exe[vm[1].pos + vm[1].off]
			until not cmd or cmd[1] ~= "choice"
			-- Break until answered.
			self.choices = cmds
			self:emit_event(self.object, {choices = choices})
			self.user = nil
			return true
		end,
		["default death check"] = function(vm, c)
			if self.object.dead then
				self.object:loot(self.user)
				for i = #vm,1,-1 do vm[i] = nil end
			else
				vm[1].pos = vm[1].pos + 1
			end
		end,
		exit = function(vm, c)
			for i = #vm,1,-1 do vm[i] = nil end
		end,
		effect = function(vm, c)
			Server:object_effect(self.object, c[2])
			vm[1].pos = vm[1].pos + 1
		end,
		["effect player"] = function(vm, c)
			Server:object_effect(self.user, c[2])
			vm[1].pos = vm[1].pos + 1
		end,
		["flag"] = function(vm, c)
			Server.quest_database:set_dialog_flag(c[2], "true")
			vm[1].pos = vm[1].pos + 1
		end,
		["flag clear"] = function(vm, c)
			Server.quest_database:set_dialog_flag(c[2], nil)
			vm[1].pos = vm[1].pos + 1
		end,
		["func"] = function(vm, c)
			local f = loadstring("return function(q)\n" .. c[2] .. "\nend")
			vm[1].pos = vm[1].pos + 1
			if f then
				local e,g = pcall(f)
				if g then g(self) end
			end
		end,
		["give player item"] = function(vm, c)
			vm[1].pos = vm[1].pos + 1
			local s = Itemspec:find{name = c[2]}
			if not s then return end
			local o = Item{spec = s, count = c.count}
			if self.user.inventory:merge_or_drop_object(o) then
				self.user:send_message("Received " .. c[2])
			else
				self.user:send_message("Received " .. c[2] .. " but couldn't carry it")
			end
		end,
		info = function(vm, c)
			-- Break until answered.
			self.choices = "info"
			self:emit_event(self.object, {message = string.format("(%s)", c[2])})
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
		notification = function(vm, c)
			vm[1].pos = vm[1].pos + 1
			Game.messaging:server_event("notification", self.user.client, c[2])
		end,
		quest = function(vm, c)
			local q = Server.quest_database:find_quest_by_name(c[2])
			if q then q:update(c) end
			vm[1].pos = vm[1].pos + 1
		end,
		random = function(vm, c)
			local o = math.random(2, #c)
			vm[1].pos = vm[1].pos + 1
			table.insert(vm, 1, {exe = c, off = o - 1, pos = 1, len = 1})
		end,
		["random quest"] = function(vm, c)
			vm[1].pos = vm[1].pos + 1
			local branch = self:create_random_quest_branch(c[2], c.difficulty)
			if branch then
				table.insert(vm, 1, {exe = branch, off = 1, pos = 1, len = #branch - 1})
			end
		end,
		["remove player item"] = function(vm, c)
			vm[1].pos = vm[1].pos + 1
			local o = self.user.inventory:get_object_by_name(c[2])
			if o then
				self.user:send_message("Lost " .. c[2])
				o:subtract(1)
				table.insert(vm, 1, {exe = c, off = 2, pos = 1, len = 1})
			elseif #c >= 3 then
				table.insert(vm, 1, {exe = c, off = 3, pos = 1, len = 1})
			end
		end,
		["require player item"] = function(vm, c)
			vm[1].pos = vm[1].pos + 1
			local o = self.user.inventory:get_object_by_name(c[2])
			if o then
				table.insert(vm, 1, {exe = c, off = 2, pos = 1, len = 1})
			elseif #c >= 3 then
				table.insert(vm, 1, {exe = c, off = 3, pos = 1, len = 1})
			end
		end,
		say = function(vm, c)
			-- Publish the line.
			self.choices = "line"
			self:emit_event(self.object, {character = c[2], message = c[3]})
			self.object:animate("talk")
			self.user = nil
			return true
		end,
		["spawn object"] = function(vm, c)
			-- Spawn the object.
			local spec1 = Actorspec:find{name = c[2]}
			local spec2 = Itemspec:find{name = c[2]}
			local spec3 = Obstaclespec:find{name = c[2]}
			local object
			if spec1 then
				object = Actor{spec = spec1, random = true}
			elseif spec2 then
				object = Item{spec = spec2, random = true}
			elseif spec3 then
				object = Obstacle{spec = spec3, random = true}
			end
			-- Set the position.
			if object then
				object:set_position(select_spawn_position(c))
				if type(c.rotation) == "number" then
					object:set_rotation(Quaternion{axis = Vector(0,1), angle = c.rotation / math.pi * 180})
				elseif type(c.rotation) == "table" then
					object:set_rotation(c.rotation)
				end
				object:set_visible(true)
			end
			-- Create the marker.
			if object and c.assign_marker then
				local name = c.assign_marker
				local marker
				local index = 1
				while Marker:find{name = name} do
					name = string.format("%s(%d)", c.assign_marker, index)
					index = index + 1
				end
				object.marker = Marker{name = name, position = object:get_position(), target = object:get_id()}
				object.marker:unlock()
			end
			vm[1].pos = vm[1].pos + 1
		end,
		["spawn pattern"] = function(vm, c)
			local pat = Patternspec:find{name = c[2]}
			if pat then
				local pos = select_spawn_position(c):copy():multiply(Voxel.tile_scale):subtract(pat.size * 0.5):round()
				if c.erase_tiles then
					Voxel:fill_region{point = pos, size = pat.size}
				end
				Voxel:place_pattern{name = c[2], point = pos, rotation = c.rotation}
			end
			vm[1].pos = vm[1].pos + 1
		end,
		teleport = function(vm, c)
			self.user:teleport(c)
			vm[1].pos = vm[1].pos + 1
		end,
		trade = function(vm, c)
			Server.trading:start(self.user, self.object)
			for i = #vm,1,-1 do vm[i] = nil end
		end,
		["unlock marker"] = function(vm, c)
			local m = Marker:find{name = c[2]}
			if m and not m.unlocked then
				m:unlock()
			end
			vm[1].pos = vm[1].pos + 1
		end,
		["unlock reward"] = function(vm, c)
			Server.unlocks:unlock_random()
			vm[1].pos = vm[1].pos + 1
		end,
		["var"] = function(vm, c)
			self.object:set_dialog_variable(c[2], "true")
			vm[1].pos = vm[1].pos + 1
		end,
		["var clear"] = function(vm, c)
			self.object:set_dialog_variable(c[2], nil)
			vm[1].pos = vm[1].pos + 1
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
			if brk then return true end
		end
	end
	-- Reset at end.
	Server.dialogs:cancel(self.object)
end

return Dialog


