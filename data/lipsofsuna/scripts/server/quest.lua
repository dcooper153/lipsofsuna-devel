require "common/eventhandler"

--- Marks the quest as active.
-- @param clss Quest class.
-- @param args Arguments.<ul>
--   <li>id: Quest id.</li>
--   <li>name: Quest name.</li>
--   <li>quest: Quest.</li></ul>
Quest.activate = function(clss, args)
	-- Find the quest.
	local q = args.quest or clss:find(args)
	if not q then return end
	-- Set the status and inform clients.
	if q.status == "unused" or q.status == "inactive" then
		q.status = "active"
		q:send{all = true}
	end
end

--- Marks the quest as completed.
-- @param clss Quest class.
-- @param args Arguments.<ul>
--   <li>id: Quest id.</li>
--   <li>name: Quest name.</li>
--   <li>quest: Quest.</li></ul>
Quest.complete = function(clss, args)
	-- Find the quest.
	local q = args.quest or clss:find(args)
	if not q then return end
	-- Set the status and inform clients.
	if q.status ~= "completed" then
		q.status = "completed"
		q:send{all = true}
	end
end

--- Sets the active map marker of the quest.
-- @param self Quest.
-- @param name Marker name or nil.
Quest.marker = function(self, name)
	local m = Marker:find{name = name}
	if self.marker_data ~= m then
		self.marker_data = m
		self:send_marker{all = true}
	end
end

--- Sends the quest status to a client or clients.
-- @param self Quest.
-- @param args Arguments.<ul>
--   <li>all: True to send to all clients.</li>
--   <li>client: Client.</li></ul>
Quest.send = function(self, args)
	-- Skip inactive quests.
	if self.status ~= "active" and self.status ~= "completed" then return end
	-- Create update packet.
	local c = self.status == "completed" and 1 or 0
	local p = Packet(packets.QUEST_STATUS, "uint32", self.id, "uint8", c, "string", self.text)
	-- Send the packet to clients.
	if args.all then
		for k,v in pairs(Player.clients) do
			v:send{packet = p}
		end
	elseif args.client then
		args.client:send{packet = p}
	end
	-- Send the mark if one is set.
	self:send_marker(args)
end

--- Sends the map marker of the quest to a client or clients.
-- @param self Quest.
-- @param args Arguments.<ul>
--   <li>all: True to send to all clients.</li>
--   <li>client: Client.</li></ul>
Quest.send_marker = function(self, args)
	-- Skip inactive quests.
	if self.status ~= "active" and self.status ~= "completed" then return end
	-- Create marker packet.
	local pos = self.marker_data and self.marker_data.position or Vector()
	local p = Packet(packets.QUEST_MARKER, "uint32", self.id,
		"float", pos.x, "float", pos.y, "float", pos.z)
	-- Send the packet to clients.
	if args.all then
		for k,v in pairs(Player.clients) do
			v:send{packet = p}
		end
	elseif args.client then
		args.client:send{packet = p}
	end
end

--- Updates the status, progress, and text of the quest.
-- @param self Quest.
-- @param args Arguments.<ul>
--   <li>progress: Progress number or nil.</li>
--   <li>status: Quest status or nil.</li>
--   <li>text: Quest text or nil.</li></ul>
Quest.update = function(self, args)
	-- Update the status.
	local ch_p = false
	local ch_s = false
	local ch_t = false
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
	-- Inform clients.
	if ch_s or ch_t then
		self:send{all = true}
	end
end
