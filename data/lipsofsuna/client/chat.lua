require "common/chatcommand"

-- Dump debug information.
ChatCommand{pattern = "^/debug_dump$", func = function(matches)
	Program:debug_dump()
end}

-- Show client stats.
ChatCommand{pattern = "^/client_stats$", func = function(matches)
	-- Collect script object stats.
	local userdata = 0
	local dicttype = {}
	for k,v in pairs(__userdata_lookup) do
		local type = v.class_name or "???"
		userdata = userdata + 1
		dicttype[type] = (dicttype[type] or 0) + 1
	end
	local listtype = {}
	for k,v in pairs(dicttype) do
		table.insert(listtype, string.format("%s: %d", k, v))
	end
	table.sort(listtype)
	local numactive = 0
	for k in pairs(Object.dict_active) do
		numactive = numactive + 1
	end
	-- Collect various memory stats.
	local models = 0
	for k,v in pairs(__userdata_lookup) do
		if v.class_name == "Model" then
			models = models + v.memory_used
		end
	end
	local stats = Render.stats
	-- Store the stats into a string.
	local objects = ""
	for k,v in ipairs(listtype) do
		objects = string.format("%s\n%s", objects, v)
	end
	Client.data.admin.client_stats = string.format([[
FPS: %.2f
Database memory: %d kB
Script memory: %d kB
Terrain memory: %d kB
Model memory: %d kB
Update tick: %d ms
Event tick: %d ms
Render tick: %d ms
Rendered batches: %d
Rendered faces: %d
Allocated meshes: %d : %dkB
Allocated skeletons: %d
Allocated textures: %d/%d : %dkB
Allocated materials: %d/%d
Active objects: %d
Userdata: %d
%s
]], Program.fps, Database.memory_used / 1024, collectgarbage("count") / 1024, Voxel.memory_used / 1024, models / 1024,
1000 * Program.profiling.update, 1000 * Program.profiling.event, 1000 * Program.profiling.render,
stats.batch_count, stats.face_count, stats.mesh_count, stats.mesh_memory / 1000, stats.skeleton_count,
stats.texture_count_loaded, stats.texture_count, stats.texture_memory / 1000,
stats.material_count_loaded, stats.material_count, numactive, userdata, objects)
	-- Activate the stats state.
	Ui.state = "admin/client-stats"
end}

-- Show player position.
ChatCommand{pattern = "^/position$", func = function(matches)
	local object = Client.player_object
	if not object then return end
	local pos = object.position
	local str = string.format("Position: %.2f %.2f %.2f", pos.x, pos.y, pos.z)
	Client:append_log(str)
	print(str)
end}

-- Show server stats.
ChatCommand{pattern = "^/server_stats$", func = function(matches)
	-- Request stats from the server.
	Network:send{packet = Packet(packets.ADMIN_STATS)}
end}

-- Lua string execution.
ChatCommand{pattern = "^/lua (.*)$", func = function(matches)
	-- Request stats from the server.
	local func,err = loadstring(matches[1])
	if err then
		Client:append_log(err)
	else
		xpcall(func, function(err)
			print(debug.traceback("ERROR: " .. err))
		end)
	end
end}

-- Normal chat.
ChatCommand{pattern = ".*", func = function(matches)
	Network:send{packet = Packet(packets.PLAYER_CHAT, "string", matches[1])}
end}
