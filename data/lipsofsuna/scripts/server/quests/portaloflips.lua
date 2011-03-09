--[[
Quest progress:
 0: Inactive.
 1: Completed.
Completion conditions:
 * Find the portal and access it.
--]]

Dialog{name = "portal of lips", unique = true,
main = function(self)
	local quest = Quest:find{name = "Portal of Lips"}
	if not quest then return end
	if quest.progress == 0 then
		self:line("You have activated the portal of Lips.")
		quest:update{status = "completed", progress = 1, marker = "portal of lips", text = "The portal of Lips has been activated."}
	else
		local q1 = Quest:find{name = "Portal of Midguard"}
		if q1 and q1.status == "completed" then
			self.user:teleport{marker = "portal of midguard"}
		else
			self:line("The other end of the portal is closed.")
		end
	end
end}
