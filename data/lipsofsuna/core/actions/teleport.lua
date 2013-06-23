Actionspec{
	name = "teleport",
	label = "Teleport",
	func = function(self, user)
		if not Main.dialogs then return end
		if Main.dialogs:execute(self, user) then
			Main.messaging:server_event("object dialog", user.client, self:get_id())
		end
	end}
