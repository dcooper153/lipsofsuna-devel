Unittest:add(1, "lipsofsuna", "terrain manager", function()
	local TerrainManager = require("core/terrain/terrain-manager")
	-- Initialization.
	local mgr = TerrainManager(16, 1)
	assert(mgr)
	assert(mgr.chunk_size == 16)
	assert(mgr.grid_size == 1)
	-- TODO
end)

Unittest:add(1, "lipsofsuna", "terrain manager: loading and unloading", function()
	local TerrainManager = require("core/terrain/terrain-manager")
	local Vector = require("system/math/vector")
	-- Loading.
	local mgr = TerrainManager(16, 2, nil, false, true, true)
	mgr:load_chunk(0, 0)
	assert(type(mgr.chunks[0]) == "table")
	-- Unloading.
	mgr:unload_chunk(0, 0)
	assert(type(mgr.chunks[0]) == "nil")
	-- Refreshing.
	mgr:refresh_chunks_by_point(Vector(16, 0, 16), 32)
	assert(type(mgr.chunks[0]) == "table")
	assert(type(mgr.chunks[1]) == "table")
	assert(type(mgr.chunks[0 + 0xFFFF]) == "table")
	assert(type(mgr.chunks[1 + 0xFFFF]) == "table")
	local count = 0
	for k,v in pairs(mgr.chunks) do count = count + 1 end
	assert(count == 4)
end)

Unittest:add(1, "lipsofsuna", "terrain manager: coordinate system", function()
	local TerrainManager = require("core/terrain/terrain-manager")
	local mgr = TerrainManager(16, 10)
	assert(mgr)
	assert(mgr.chunk_size == 16)
	assert(mgr.grid_size == 10)
	assert(mgr:get_chunk_id_by_xz(0, 0) == 0)
	assert(mgr:get_chunk_id_by_xz(5, 0) == 0)
	assert(mgr:get_chunk_id_by_xz(16, 0) == 1)
	assert(mgr:get_chunk_id_by_xz(160, 0) == 10)
	assert(mgr:get_chunk_id_by_xz(0, 5) == 0)
	assert(mgr:get_chunk_id_by_xz(0, 16) == 0xFFFF)
	assert(mgr:get_chunk_id_by_xz(0, 160) == 10 * 0xFFFF)
	local test_id_xz = function(id, x, z)
		local xt,zt = mgr:get_chunk_xz_by_id(id)
		return x == xt and z == zt
	end
	assert(test_id_xz(0, 0, 0))
	assert(test_id_xz(1, 16, 0))
	assert(test_id_xz(10, 160, 0))
	assert(test_id_xz(0xFFFF, 0, 16))
	assert(test_id_xz(10 * 0xFFFF, 0, 160))
	assert(mgr:get_chunk_id_by_point(160, 160) == mgr:get_chunk_id_by_xz(16, 16))
	assert(mgr:get_chunk_id_by_point(168, 328) == mgr:get_chunk_id_by_xz(16, 32))
	local test_pt_xz = function(wx, wz, x, z)
		local xt,zt = mgr:get_chunk_xz_by_point(wx, wz)
		return x == xt and z == zt
	end
	assert(test_pt_xz(0, 0, 0, 0))
	assert(test_pt_xz(160, 320, 16, 32))
	assert(test_pt_xz(16333, 3205, 1632, 320))
end)
