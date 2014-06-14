local ActionSpec = require("core/specs/action")
local ChatCommand = require("core/chat/chat-command")
local ModifierSpec = require("core/specs/modifier")
local Skillspec = require("core/specs/skill")

ChatCommand{
	name = "unlock_action",
	description = "Unlock an action.",
	pattern = "^/unlock_action (.*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		local spec = Main.specs:find_by_name("Actionspec", matches[1])
		if spec then
			Main.unlocks:unlock("action", matches[1])
		else
			player:send_message(string.format("No such action %q.", matches[1]))
		end
	end}

ChatCommand{
	name = "unlock_all",
	description = "Unlock all skills, spell types or spell effects.",
	pattern = "^/unlock_all$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		Main.unlocks:unlock_all()
	end}

ChatCommand{
	name = "unlock_random",
	description = "Unlock a random skill, spell type or spell effect.",
	pattern = "^/unlock_random$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		Main.unlocks:unlock_random()
	end}

ChatCommand{
	name = "unlock_modifier",
	description = "Unlock a modifier.",
	pattern = "^/unlock_modifier (.*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		local spec = Main.specs:find_by_name("ModifierSpec", matches[1])
		if spec then
			Main.unlocks:unlock("modifier", matches[1])
		else
			player:send_message(string.format("No such modifier %q.", matches[1]))
		end
	end}

ChatCommand{
	name = "unlock_skill",
	description = "Unlock a skill.",
	pattern = "^/unlock_skill (.*)$",
	permission = "admin",
	handler = "server",
	func = function(player, matches)
		local spec = Main.specs:find_by_name("Skillspec", matches[1])
		if spec then
			Main.unlocks:unlock("skill", matches[1])
		else
			player:send_message(string.format("No such skill %q.", matches[1]))
		end
	end}
