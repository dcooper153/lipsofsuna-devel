--[[
Quest progress:
 0: Inactive.
 1: Completed.
Completion conditions:
 * Access the Sanctuary device.
--]]

Dialog{name = "sanctuary", unique = true,
main = function(self)
	local quest = Quest:find{name = "Sanctuary"}
	if not quest then return end
	if quest.progress == 0 then
		self:line("You have activated the Sanctuary.")
		quest:update{status = "completed", progress = 1, text = "The Sanctuary has been activated."}
	else
		self:line("The Sanctuary is already active.")
	end
end}
