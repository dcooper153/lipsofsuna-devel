--[[
Quest progress:
 * 0x0: Learned about the root grove.
 * 0x1: Talked to Chara and learned her name.
 * 0x2: Learned about the awakening.
 * 0x4: Learned about the importance of Erinyes.
 * 0x8: Asked Chara to help defeat Erinyes.
Completion conditions:
 * 0x8: Asked Chara to help defeat Erinyes.
Related quests:
 * Mourning Adventurer: Learn about this quest.
 * Chara's Illusion: Started when this quest is completed.
--]]

Dialog{name = "chara", unique = true,
main = function(self)
	local quest = Quest:find{name = "Roots of World"}
	if not quest then return end
	self:line("Greetings, low-life.")
	local choices = {
		"What are you?",
		"What are Seirei?",
		"What are you doing here?",
		"Why did the World Tree wither?",
		"How were you awaken and by whom?",
		"What kind of a Seirei is Erinyes?",
		"Why did you choose to awaken?",
		"Tell me exactly what the culprit did to awaken Erinyes.",
		"We will stop Erinyes. Will you help us?",
		"Goodbye"}
	while true do
		-- Get allowed choices.
		local allowed = {}
		table.insert(allowed, choices[1])
		if Bitwise:bchk(quest.progress, 1) then
			table.insert(allowed, choices[2])
			table.insert(allowed, choices[3])
			table.insert(allowed, choices[4])
		end
		if Bitwise:bchk(quest.progress, 2) then
			table.insert(allowed, choices[5])
		end
		if Bitwise:bchk(quest.progress, 4) then
			table.insert(allowed, choices[6])
			table.insert(allowed, choices[7])
			table.insert(allowed, choices[8])
			table.insert(allowed, choices[9])
		end
		table.insert(allowed, choices[10])
		-- Prompt for a choice and show the dialog branch.
		local a = self:choice(allowed)
		if a == choices[1] then
			self:line("Low-lifes never have any kind of manners.")
			self:line("Talking to a lady as if she were an object, hmph.")
			self:line("And of all ladies, to Chara, the Seirei of the Roots of the World Tree.")
			quest:update{status = "active", progress = Bitwise:bor(quest.progress, 1), text =
				"We have discovered Chara, the Seirei of the Roots of the World Tree."}
		elseif a == choices[2] then
			self:line("Meaningless glowing things trapped into a meaningless, ever-repeating cycle of time.")
			self:choice{"No, really."}
			self:line("That is all. If you insist, you can call me the God, energy, faith, nature or the universe. I am all of those, yet nothing but a puppet of the cycle of time itself.")
		elseif a == choices[3] then
			self:line("Nothing meaningful, at all. This is how far I got before I stopped caring and this is how far I'll go in this era.")
			self:line("To make minch of the low-lifes, fly to the depths to avoid them, to stay here and listen to their blathering...")
			self:line("The choice makes no difference. The outcome is always the same.")
		elseif a == choices[4] then
			self:line("Because it made no difference anymore.")
			self:line("Whether the tree withers or not, the world always perishes the same way.")
			self:line("When the lowest of the low-lifes awakened Erinyes, it all already came to the end.")
			quest:update{status = "active", progress = Bitwise:bor(quest.progress, 2), text =
				"Chara, the Seirei of the Roots of the World Tree, has told us that " ..
				"the world is about to end because a Seirei named Erinyes has been awakened."}
		elseif a == choices[5] then
			self:line("No big surprise there, it was one of you low-lifes, again in the right place in the right time.")
			self:line("You low-lifes sure know how to produce such prime individuals.")
			self:line("The lowest of the low always becomes the vessel of Erinyes and gets the idea to awaken him. Truly, it is marvelous how it happens every time.")
			self:line("Had it been any other Seirei, the low-life would have gutted himself in vain, but no, it's always Erinyes.")
			self:line("And because it was Erinyes, it's all of us, eventually.")
			quest:update{status = "active", progress = Bitwise:bor(quest.progress, 4), text =
				"Chara, the Seirei of the Roots of the World Tree, has told us that " ..
				"the world is about to end because a madman of some kind awakened the " ..
				"Seirei named Erinyes that was sleeping within him. The awakening of " ..
				"Erinyes and his ability to awaken the rest of the Seirei seems to be the " ..
				"very reason why the world is withering."}
		elseif a == choices[6] then
			self:line("Erinyes is the champion of chaos, the fundamental force of change, the bell of awakening, the violent creation and the violent destruction.")
			self:line("His awakening marks the ultimate and irreversible destruction as the existence is consumed by the chaos born from him awakening each and every Seirei.")
		elseif a == choices[7] then
			self:line("Would you choose not to awaken if someone poured various fluids right on your face?")
			local b = self:choice{"Yes.", "No."}
			if b == "Yes." then
				self:line("That's right, in a situation like that, you don't want to stop them by showing that you're awake.")
				self:line("Silently, you hope that they'd pour more of their icky fluids in various places.")
				self:line("Truly, the low-life nature at its finest.")
				self:line("Low-lifes of your kind sure will be in ecstasy when death pours the fluids all over you.")
			else
				self:line("Right, and neither do you keep sleeping when they do that and Erinyes growls right before you in addition.")
			end
		elseif a == choices[8] then
			self:line("You low-lifes really enjoy this part, don't you? Oh, you do, immensely. I know it.")
			self:line("To get the idea, try to imagine doing all the fun stuff in the same day.")
			self:line("Fun stuff like lethal chemicals, drilling holes to your skull, eating your own flesh, bleeding yourself dry and more. The low-life sure had guts, at least until he rid himself of most of them.")
		elseif a == choices[9] then
			self:line("Oh, sure I will. This is so totally new and exciting that I'm overjoyed to tears.")
			self:line("Except that it isn't, and I don't care even a bit.")
			if not Bitwise:bchk(quest.progress, 8) then
				self:line("You low-lifes always have a big mouth but no substance.")
				self:line("See for yourself what you're trying to defeat.")
				quest:update{status = "active", progress = Bitwise:bor(quest.progress, 8), text =
					"Chara, the Seirei of the Roots of the World Tree, has told us that " ..
					"the world is about to end because a madman of some kind awakened the " ..
					"Seirei named Erinyes that was sleeping within him. The awakening of " ..
					"Erinyes and his ability to awaken the rest of the Seirei seems to be the " ..
					"very reason why the world is withering. Chara has opened a portal in " ..
					"her root grove to show what awaits us."}
				--TODO: Open the portal.
				local q1 = Quest:find{name = "Chara's Illusion"}
				if q1 and q1.status == "inactive" then
					q1:update{status = "active", progress = 0, marker = "chara's portal", text =
						"Chara has opened a portal in her root grove for us. She has told us " ..
						"to step in to learn about defeating Erinyes."}
				end
			else
				self:line("Walk into the portal or walk away. It makes no difference.")
			end
		else break end
	end
end}
