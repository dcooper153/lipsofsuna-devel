--[[
Quest progress:
 * 0x01: Learn about the missing companion.
 * 0x02: Found the companion.
 * 0x04: Escorted the lost friend to the town. (TODO: Not implemented, requires the AI mode "follow")
 * 0x08: The lost friend is dead.
 * 0x10: Told the character in the town that the companion is dead.
 * 0xFF: The town character is dead.
Completion conditions:
 * 0x04: Escorted the lost friend to the town.
 * 0x10: Told the character in the town that the companion is dead.
 * 0xFF: The town character is dead.
Related quests:
 * Roots of World: Provides information on that quest.
--]]

Dialog{name = "mourningadventurer_townchar", unique = true,
die = function(self)
	local quest = Quest:find{name = "Mourning Adventurer"}
	if not quest then return end
	if Bitwise:band(quest.progress, 0x14) == 0 then
		quest:update{status = "completed", progress = Bitwise:bor(quest.progress, 0xFF), text =
			"The mourning adventurer is dead."}
	end
end,
main = function(self)
	local quest = Quest:find{name = "Mourning Adventurer"}
	if not quest then return end
	if self.object.dead then
		self:line("(She's dead.)")
		return
	end
	if Bitwise:bchk(quest.progress, 0x4) then
		self:line("Hello.")
	elseif Bitwise:bchk(quest.progress, 0x10) then
		self:line("I want to be alone for a bit.")
		return
	else
		self:line("Please, I need your help!")
	end
	local choices = {
		"What's wrong?",
		"Where did you get ambushed?",
		"Why don't you try to rescue your friend yourself?",
		"I have found your friend, but...",
		"Goodbye"}
	while true do
		-- Get allowed choices.
		local allowed = {}
		if Bitwise:band(quest.progress, 0x18) == 0 then
			table.insert(allowed, choices[1])
		end
		if Bitwise:bchk(quest.progress, 1) then
			table.insert(allowed, choices[2])
			table.insert(allowed, choices[3])
		end
		if Bitwise:bchk(quest.progress, 8) then
			table.insert(allowed, choices[4])
		end
		table.insert(allowed, choices[5])
		-- Prompt for a choice and show the dialog branch.
		local a = self:choice(allowed)
		if a == choices[1] then
			self:line("Me and my friend were ambushed by monsters when we were exploring the dungeons.")
			self:line("I barely managed to escape but she's still there somewhere!")
			self:line("Please, you need to find her!")
			if quest.progress == 0 then
				quest:update{status = "active", progress = Bitwise:bor(quest.progress, 1), text =
					"A person in the town of Lips has requested us to find her friend who " ..
					"got separated from her as they were ambushed by monsters."}
			end
		elseif a == choices[2] then
			self:line("It wasn't far from the town.")
			self:line("There was a weird grove of roots illuminated by a bright moving light.")
			self:line("We were going to take a look at the place but then the monsters attacked.")
			local q2 = Quest:find{name = "Roots of World"}
			if q2 and q2.status == "inactive" then
				q2:update{status = "active", progress = 0, text =
					"A person in the town of Lips has told us of weird grove of roots " ..
					"illuminated by a brigh glow. Perhaps we should search the grove " ..
					"for clues about the fate of the World Tree."}
			end
		elseif a == choices[3] then
			self:line("I wish I could but I don't have the courage.")
			self:line("I'm no warrior, never claimed to be. They just sent me here without even telling that we have to fight!")
			self:line("Just seeing those horrific monsters scares me so much I can't but run.")
		elseif a == choices[4] then
			self:line("Sigh, I take it that she's dead.")
			self:line("Don't say anything, it isn't your fault.")
			if not Bitwise:bchk(quest.progress, 0x10) then
				self:line("You tried to help so let me teach you something that might help you in your journey.")
				quest:update{status = "completed", progress = Bitwise:bor(quest.progress, 0x10), text =
					"We have told waiting in the town of Lips that her friend was killed " ..
					"by the monsters that attacked them."}
				Feat:unlock() --TODO: Shouldn't be a combat ability.
				return
			end
			self:line("I want to be left alone now.")
		else return end
	end
end}

Dialog{name = "mourningadventurer_lostchar", unique = true,
main = function(self)
	local quest = Quest:find{name = "Mourning Adventurer"}
	if not quest then return end
	if not Bitwise:bchk(quest.progress, 0x8) then
		quest:update{status = "active", progress = Bitwise:bor(quest.progress, 0x8), text =
			"The lost adventurer is dead. We should let her worried friend know it."}
	end
	self:line("(She's dead.)")
end}
