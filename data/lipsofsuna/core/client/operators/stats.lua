local Class = require("system/class")
local Database = require("system/database")
local Render = require("system/render")

Operators.stats = Class("StatsOperator")
Operators.stats.data = {}

Operators.stats.get_client_stats_text = function(self)
	return self.data.client_stats or ""
end

Operators.stats.update_client_stats = function(self, show_objects)
	-- Collect various memory stats.
	local models = 0
	for k,v in pairs(__userdata_lookup) do
		if v.class_name == "Model" then
			models = models + v:get_memory_used()
		end
	end
	local stats = Render:get_stats()
	-- Store the stats into a string.
	self.data.client_stats = string.format([[
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
Allocated attachments: %d
Allocated entities: %d
Allocated meshes: %d : %dkB
Allocated skeletons: %d
Allocated textures: %d/%d : %dkB
Allocated materials: %d/%d
]], Program:get_fps(), Database:get_memory_used() / 1024, collectgarbage("count") / 1024, Voxel:get_memory_used() / 1024, models / 1024,
1000 * (Program.profiling.update or 0), 1000 * (Program.profiling.event or 0), 1000 * (Program.profiling.render or 0),
stats.batch_count, stats.face_count, stats.attachment_count, stats.entity_count, stats.mesh_count,
stats.mesh_memory / 1000, stats.skeleton_count, stats.texture_count_loaded, stats.texture_count,
stats.texture_memory / 1000, stats.material_count_loaded, stats.material_count)
	-- Collect script object stats.
	if show_objects then
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
		for k in pairs(Game.objects:get_active_objects()) do
			numactive = numactive + 1
		end
		local objects = ""
		for k,v in ipairs(listtype) do
			objects = string.format("%s\n%s", objects, v)
		end
		self.data.client_stats = string.format([[
%s
Active objects: %d
Userdata: %d
%s
]], self.data.client_stats, numactive, userdata, objects)
	end
end
