local Class = require("system/class")
local Database = require("system/database")
local Model = require("system/model")
local Render = require("system/render")

Operators.stats = Class("StatsOperator")
Operators.stats.data = {}

Operators.stats.get_client_stats_text = function(self)
	return self.data.client_stats or ""
end

Operators.stats.update_client_stats = function(self, show_objects)
	-- Collect various memory stats.
	local models = Model:get_total_model_count()
	local model_mem = Model:get_total_memory_used()
	local stats = Render:get_stats()
	-- Store the stats into a string.
	self.data.client_stats = string.format([[
FPS: %.2f
Database memory: %d kB
Script memory: %d kB
Terrain chunks: %d
Terrain memory: %d kB
Model count: %d
Model memory: %d kB

%s

Rendered batches: %d
Rendered faces: %d
Allocated attachments: %d
Allocated entities: %d
Allocated meshes: %d : %dkB
Allocated skeletons: %d
Allocated textures: %d/%d : %dkB
Allocated materials: %d/%d
]], Program:get_fps(), Database:get_memory_used() / 1024, collectgarbage("count") / 1024,
Main.terrain.terrain:get_chunk_count(), Main.terrain.terrain:get_memory_used() / 1024,
models, model_mem / 1024, Main.timing:get_profiling_string(),
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
		for k in pairs(Main.objects:get_active_objects()) do
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
