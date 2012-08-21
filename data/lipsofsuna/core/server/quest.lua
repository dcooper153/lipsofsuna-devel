require "common/marker"
local Class = require("system/class")

local Quest = Class("Quest")

--- Creates a new quest.
-- @param clss Quest class.
-- @param name Quest name.
-- @param status Status string, or nil.
-- @param text Description text, or nil.
-- @param marker Marker name, or nil.
Quest.new = function(clss, name, status, text, marker)
	local self = Class.new(clss)
	self.name = name
	self.status = status or "inactive"
	self.text = text
	self.marker = marker
	return self
end

--- Sends the quest status to the given client.
-- @param self Quest.
-- @param client Client.
-- @param status True to send the status.
-- @param marker True to send the marker.
Quest.send_to_client = function(self, client, status, marker)
	if self.status ~= "active" and self.status ~= "completed" then return end
	if status then
		Game.messaging:server_event("update quest status", client, self.name, self.status, self.text)
	end
	if marker then
		Game.messaging:server_event("update quest marker", client, self.name, self.marker)
	end
end

--- Updates the status, progress, and text of the quest.
-- @param self Quest.
-- @param args Arguments.<ul>
--   <li>progress: Progress number or nil.</li>
--   <li>marker: Map marker name or nil.</li>
--   <li>status: Quest status or nil.</li>
--   <li>text: Quest text or nil.</li></ul>
Quest.update = function(self, args)
	-- Update the status.
	local ch_m = false
	local ch_p = false
	local ch_s = false
	local ch_t = false
	if args.marker and self.marker ~= args.marker then
		local m = Marker:find{name = args.marker}
		if m then
			if not m.unlocked then
				m:unlock()
			end
			self.marker = args.marker
			ch_m = true
		end
	end
	if args.progress and self.progres ~= args.progress then
		self.progress = args.progress
		ch_p = true
	end
	if args.status and self.status ~= args.status then
		self.status = args.status
		ch_s = true
	end
	if args.text and self.text ~= args.text then
		self.text = args.text
		ch_t = true
	end
	-- Inform all clients.
	for client in pairs(Server.players_by_client) do
		self:send_to_client(client, ch_s or ch_t, ch_m)
	end
	-- Save changes.
	if ch_m or ch_p or ch_s or ch_t then
		Server.quest_database:save_quest(self)
	end
end

return Quest
