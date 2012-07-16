Actionspec{
	name = "teleport",
	label = "Teleport",
	func = function(self, user)
		if self.dialog then return end
		local dialog = Dialog{object = self, user = user}
		if not dialog then return end
		self.dialog = dialog
		if self.dialog:execute() then
			Game.messaging:server_event("object dialog", user.client, self.id)
		end
	end}
