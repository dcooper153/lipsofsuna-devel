Marker.unlock = function(self)
	if self.unlocked then return end
	self.unlocked = true
	for k,v in pairs(Server.players_by_client) do
		Game.messaging:server_event("create marker", k, self.name, self.position)
	end
	Server.serialize:save_marker(self)
end
