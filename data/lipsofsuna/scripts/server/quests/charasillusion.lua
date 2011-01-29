--[[
Quest progress:
 0: Inactive.
 1: Completed.
Completion conditions:
 * Find the portal and access it.
--]]

Dialog{name = "portal of illusion", unique = true,
main = function(self)
	self.user:teleport{marker = "chara's portal"}
end}

Dialog{name = "chara's portal", unique = true,
main = function(self)
	local quest = Quest:find{name = "Chara's Illusion"}
	if quest and quest.status ~= "inactive" then
		self.user:teleport{marker = "portal of illusion"}
	else
		self:line("The portal doesn't seem to be active.")
	end
end}
