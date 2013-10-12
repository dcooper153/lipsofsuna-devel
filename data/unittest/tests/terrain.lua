local assert_stick = function(stick, mat, height, y00, y10, y01, y11)
	assert(stick[1] == mat)
	assert(stick[2] == height)
	assert(stick[3] == y00)
	assert(stick[8] == y10)
	assert(stick[13] == y01)
	assert(stick[18] == y11)
end

local print_column = function(c)
	print("Column:")
	local y = 0
	for k,v in ipairs(c) do
		print(string.format("  Stick %d: y=%.2f hgt=%.2f mat=%d", k, y, v[2], v[1]))
		y = y + v[2]
		for i=3,18,5 do
			print(string.format("    Vertex y=%.2f off=%.2f spl=%.1f nml=%.2f,%.2f,%.2f", y + v[i], v[i], v[i + 1], v[i + 2], v[i + 3], v[i + 4]))
		end
	end
end

Unittest:add(1, "system", "terrain", function()
	local Terrain = require("system/terrain")
	-- Initialization.
	local t = Terrain(32, 0.5)
	assert(t)
	assert(type(t.handle) == "userdata")
	-- Loading and unloading.
	assert(not t:get_column(100, 100))
	assert(t:load_chunk(100, 100))
	local c = t:get_column(100, 100)
	assert(type(c) == "table")
	assert(c[1] == nil)
	assert(t:unload_chunk(100, 100))
	assert(not t:get_column(100, 100))
end)

Unittest:add(1, "system", "terrain: add empty", function()
	local Terrain = require("system/terrain")
	local t = Terrain(32, 0.5)
	t:load_chunk(0, 0)
	-- Adding empty to an empty column.
	t:add_stick(0, 0, 10, 5, 0)
	local c = t:get_column(0, 0)
	assert(#c == 0)
	-- Appending to an empty column.
	t:add_stick(0, 0, 0, 5, 1)
	local c = t:get_column(0, 0)
	assert(#c == 1)
	assert_stick(c[1], 1, 5, 0, 0, 0, 0)
	-- Appending empty to a non-empty column.
	t:add_stick(0, 0, 100, 5, 0)
	local c = t:get_column(0, 0)
	assert(#c == 1)
	assert_stick(c[1], 1, 5, 0, 0, 0, 0)
	-- Overwriting a column with empty.
	t:add_stick(0, 0, 0, 5, 0)
	local c = t:get_column(0, 0)
	assert(#c == 0)
	-- Appending to an empty column with padding.
	t:add_stick(1, 0, 10, 5, 1)
	local c = t:get_column(1, 0)
	assert(#c == 2)
	assert_stick(c[1], 0, 10, 0, 0, 0, 0)
	assert_stick(c[2], 1, 5, 0, 0, 0, 0)
end)

Unittest:add(1, "system", "terrain: add end", function()
	local Terrain = require("system/terrain")
	local t = Terrain(32, 0.5)
	t:load_chunk(0, 0)
	-- Replacing from the end.
	t:add_stick(0, 0, 0, 40, 3)
	t:add_stick(0, 0, 25, 15, 2)
	t:add_stick(0, 0, 35, 5, 1)
	local c = t:get_column(0, 0)
	assert(#c == 3)
	assert_stick(c[1], 3, 25, 0, 0, 0, 0)
	assert_stick(c[2], 2, 10, 0, 0, 0, 0)
	assert_stick(c[3], 1, 5, 0, 0, 0, 0)
	-- Replacing multiple from the end.
	t:add_stick(0, 0, 5, 55, 4)
	local c = t:get_column(0, 0)
	assert(#c == 2)
	assert_stick(c[1], 3, 5, 0, 0, 0, 0)
	assert_stick(c[2], 4, 55, 0, 0, 0, 0)
end)

Unittest:add(1, "system", "terrain: add middle", function()
	local Terrain = require("system/terrain")
	local t = Terrain(32, 0.5)
	t:load_chunk(0, 0)
	-- 000011112222
	--   1111
	t:add_stick(0, 0, 4, 4, 1)
	t:add_stick(0, 0, 8, 4, 2)
	t:add_stick(0, 0, 2, 4, 1)
	local c = t:get_column(0, 0)
	assert(#c == 3)
	assert_stick(c[1], 0, 2, 0, 0, 0, 0)
	assert_stick(c[2], 1, 6, 0, 0, 0, 0)
	assert_stick(c[3], 2, 4, 0, 0, 0, 0)
	-- 000011112222
	--     1111
	t:clear_column(0, 0)
	t:add_stick(0, 0, 4, 4, 1)
	t:add_stick(0, 0, 8, 4, 2)
	t:add_stick(0, 0, 4, 4, 1)
	local c = t:get_column(0, 0)
	assert(#c == 3)
	assert_stick(c[1], 0, 4, 0, 0, 0, 0)
	assert_stick(c[2], 1, 4, 0, 0, 0, 0)
	assert_stick(c[3], 2, 4, 0, 0, 0, 0)
	-- 000011112222
	--      11
	t:clear_column(0, 0)
	t:add_stick(0, 0, 4, 4, 1)
	t:add_stick(0, 0, 8, 4, 2)
	t:add_stick(0, 0, 5, 2, 1)
	local c = t:get_column(0, 0)
	assert(#c == 3)
	assert_stick(c[1], 0, 4, 0, 0, 0, 0)
	assert_stick(c[2], 1, 4, 0, 0, 0, 0)
	assert_stick(c[3], 2, 4, 0, 0, 0, 0)
	-- 000011112222
	--       1111
	t:clear_column(0, 0)
	t:add_stick(0, 0, 4, 4, 1)
	t:add_stick(0, 0, 8, 4, 2)
	t:add_stick(0, 0, 6, 4, 1)
	local c = t:get_column(0, 0)
	assert(#c == 3)
	assert_stick(c[1], 0, 4, 0, 0, 0, 0)
	assert_stick(c[2], 1, 6, 0, 0, 0, 0)
	assert_stick(c[3], 2, 2, 0, 0, 0, 0)
	-- 000011112222
	--   11111111
	t:clear_column(0, 0)
	t:add_stick(0, 0, 4, 4, 1)
	t:add_stick(0, 0, 8, 4, 2)
	t:add_stick(0, 0, 2, 8, 1)
	local c = t:get_column(0, 0)
	assert(#c == 3)
	assert_stick(c[1], 0, 2, 0, 0, 0, 0)
	assert_stick(c[2], 1, 8, 0, 0, 0, 0)
	assert_stick(c[3], 2, 2, 0, 0, 0, 0)
	-- 000011112222
	--   3333
	t:clear_column(0, 0)
	t:add_stick(0, 0, 4, 4, 1)
	t:add_stick(0, 0, 8, 4, 2)
	t:add_stick(0, 0, 2, 4, 3)
	local c = t:get_column(0, 0)
	assert(#c == 4)
	assert_stick(c[1], 0, 2, 0, 0, 0, 0)
	assert_stick(c[2], 3, 4, 0, 0, 0, 0)
	assert_stick(c[3], 1, 2, 0, 0, 0, 0)
	assert_stick(c[4], 2, 4, 0, 0, 0, 0)
	-- 000011112222
	--     3333
	t:clear_column(0, 0)
	t:add_stick(0, 0, 4, 4, 1)
	t:add_stick(0, 0, 8, 4, 2)
	t:add_stick(0, 0, 4, 4, 3)
	local c = t:get_column(0, 0)
	assert(#c == 3)
	assert_stick(c[1], 0, 4, 0, 0, 0, 0)
	assert_stick(c[2], 3, 4, 0, 0, 0, 0)
	assert_stick(c[3], 2, 4, 0, 0, 0, 0)
	-- 000011112222
	--      33
	t:clear_column(0, 0)
	t:add_stick(0, 0, 4, 4, 1)
	t:add_stick(0, 0, 8, 4, 2)
	t:add_stick(0, 0, 5, 2, 3)
	local c = t:get_column(0, 0)
	assert(#c == 5)
	assert_stick(c[1], 0, 4, 0, 0, 0, 0)
	assert_stick(c[2], 1, 1, 0, 0, 0, 0)
	assert_stick(c[3], 3, 2, 0, 0, 0, 0)
	assert_stick(c[4], 1, 1, 0, 0, 0, 0)
	assert_stick(c[5], 2, 4, 0, 0, 0, 0)
	-- 000011112222
	--       3333
	t:clear_column(0, 0)
	t:add_stick(0, 0, 4, 4, 1)
	t:add_stick(0, 0, 8, 4, 2)
	t:add_stick(0, 0, 6, 4, 3)
	local c = t:get_column(0, 0)
	assert(#c == 4)
	assert_stick(c[1], 0, 4, 0, 0, 0, 0)
	assert_stick(c[2], 1, 2, 0, 0, 0, 0)
	assert_stick(c[3], 3, 4, 0, 0, 0, 0)
	assert_stick(c[4], 2, 2, 0, 0, 0, 0)
	-- 000011112222
	--   33333333
	t:clear_column(0, 0)
	t:add_stick(0, 0, 4, 4, 1)
	t:add_stick(0, 0, 8, 4, 2)
	t:add_stick(0, 0, 2, 8, 3)
	local c = t:get_column(0, 0)
	assert(#c == 3)
	assert_stick(c[1], 0, 2, 0, 0, 0, 0)
	assert_stick(c[2], 3, 8, 0, 0, 0, 0)
	assert_stick(c[3], 2, 2, 0, 0, 0, 0)
end)

Unittest:add(1, "system", "terrain: add start", function()
	local Terrain = require("system/terrain")
	local t = Terrain(32, 0.5)
	t:load_chunk(0, 0)
	-- Replacing the from the start.
	t:add_stick(0, 0, 0, 40, 3)
	t:add_stick(0, 0, 0, 15, 2)
	t:add_stick(0, 0, 0, 5, 1)
	local c = t:get_column(0, 0)
	assert(#c == 3)
	assert_stick(c[1], 1, 5, 0, 0, 0, 0)
	assert_stick(c[2], 2, 10, 0, 0, 0, 0)
	assert_stick(c[3], 3, 25, 0, 0, 0, 0)
	-- Replacing multiple from the start.
	t:add_stick(0, 0, 0, 39, 0)
	local c = t:get_column(0, 0)
	assert(#c == 2)
	assert_stick(c[1], 0, 39, 0, 0, 0, 0)
	assert_stick(c[2], 3, 1, 0, 0, 0, 0)
	-- Replacing from the start and past the end.
	t:add_stick(0, 0, 0, 50, 5)
	local c = t:get_column(0, 0)
	assert(#c == 1)
	assert_stick(c[1], 5, 50, 0, 0, 0, 0)
end)

Unittest:add(1, "system", "terrain: add stick corners", function()
	local Terrain = require("system/terrain")
	local t = Terrain(32, 0.5)
	t:load_chunk(0, 0)
	-- Appending a stick.
	assert(t:add_stick_corners(0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 10))
	local c = t:get_column(0, 0)
	assert(#c == 2)
	assert_stick(c[1], 0, 2.5, -1.5, -0.5, 0.5, 1.5)
	assert_stick(c[2], 10, 4, -1.5, -0.5, 0.5, 1.5)
	-- Overwriting the top.
	assert(t:add_stick_corners(0, 0, 6, 6, 7, 7, 10, 7, 10, 7, 11))
	local c = t:get_column(0, 0)
	assert(#c == 3)
	assert_stick(c[1], 0, 2.5, -1.5, -0.5, 0.5, 1.5)
	assert_stick(c[2], 10, 4, -1.5, -0.5, 0.5, 0.5)
	assert_stick(c[3], 11, 2, 1.5, -1.5, 1.5, -1.5)
	-- Overwriting the middle.
	assert(t:add_stick_corners(1, 1, 0, 0, 0, 0, 10, 10, 11, 11, 1))
	local c = t:get_column(1, 1)
	assert(#c == 1)
	assert_stick(c[1], 1, 10.5, -0.5, -0.5, 0.5, 0.5)
	assert(t:add_stick_corners(1, 1, 5, 5, 5, 5, 8, 8, 10, 10, 2))
	local c = t:get_column(1, 1)
	assert(#c == 3)
	assert_stick(c[1], 1, 5, 0, 0, 0, 0)
	assert_stick(c[2], 2, 4, -1, -1, 1, 1)
	assert_stick(c[3], 1, 1.5, -0.5, -0.5, 0.5, 0.5)
	-- XXXXXXXXXXX
	assert(t:add_stick_corners(1, 0, 0, 0, 0, 0, 11, 10, 9, 10, 11))
	assert(t:add_stick_corners(1, 0, 9, 9, 10, 10, 10, 11, 9, 9, 0))
	local c = t:get_column(1, 0)
	--print_column(c)
end)

Unittest:add(1, "system", "terrain: count materials", function()
	local Terrain = require("system/terrain")
	local Vector = require("system/math/vector")
	local t = Terrain(32, 0.5)
	t:load_chunk(0, 0)
	-- Empty column.
	local r = t:count_column_materials(0, 0)
	for k,v in pairs(r) do
		assert(false)
	end
	-- Multiple results.
	t:add_stick(0, 0, 0, 2.5, 2)
	t:add_stick(0, 0, 5, 5.5, 1)
	t:add_stick(0, 0, 10.5, 0.5, 2)
	t:add_stick(0, 0, 11, 2, 3)
	local r = t:count_column_materials(0, 0)
	assert(r[0] == 2.5)
	assert(r[1] == 5.5)
	assert(r[2] == 3)
	assert(r[3] == 2)
	-- Limit range.
	local r = t:count_column_materials(0, 0, 10, 2)
	assert(not r[0])
	assert(r[1] == 0.5)
	assert(r[2] == 0.5)
	assert(r[3] == 1)
	-- Reusing the table.
	local r1 = t:count_column_materials(0, 0, 10, 2, r)
	assert(r == r1)
	assert(not r[0])
	assert(r[1] == 1)
	assert(r[2] == 1)
	assert(r[3] == 2)
	-- Counting in sphere.
	t:unload_chunk(0, 0)
	t:load_chunk(0, 0)
	t:add_stick(0, 0, 0, 4, 1)
	t:add_stick(1, 1, 0, 4, 2)
	t:add_stick(2, 2, 0, 4, 3)
	t:add_stick(3, 3, 0, 4, 4)
	local r = t:count_materials_in_sphere(Vector(0, 0, 0), 2.5)
	assert(r[0] > 20)
	assert(r[1] == 4)
	assert(r[2] < r[1])
	assert(r[3] < r[2])
	assert(not r[4])
end)

Unittest:add(1, "system", "terrain: build model", function()
	local Terrain = require("system/terrain")
	local t = Terrain(32, 0.5)
	assert(not t:build_chunk_model(0, 0))
	t:load_chunk(0, 0)
	t:add_stick(1, 1, 0, 40, 3)
	t:add_stick(1, 1, 0, 15, 2)
	t:add_stick(1, 1, 0, 5, 1)
	-- Building.
	local m = t:build_chunk_model(0, 0)
	assert(type(m) == "table")
	assert(m.class_name == "Model")
	assert(m:get_vertex_count() == 13 * 6)
	-- Finding outdated models.
	local x,z = t:get_nearest_chunk_with_outdated_model(0, 0)
	assert(x == nil)
	assert(z == nil)
	-- Loading chunks outdates neighbors.
	t:load_chunk(32, 0)
	local x,z = t:get_nearest_chunk_with_outdated_model(0, 0)
	assert(x == 0)
	assert(z == 0)
	local m = t:build_chunk_model(0, 0)
	assert(type(m) == "table")
	local x,z = t:get_nearest_chunk_with_outdated_model(32, 0)
	assert(x == nil)
	assert(z == nil)
	-- Adding sticks outdates chunks.
	t:add_stick(40, 0, 0, 40, 3)
	local x,z = t:get_nearest_chunk_with_outdated_model(0, 0)
	assert(x == 32)
	assert(z == 0)
	local m = t:build_chunk_model(32, 0)
	assert(type(m) == "table")
	local x,z = t:get_nearest_chunk_with_outdated_model(0, 0)
	assert(x == nil)
	assert(z == nil)
	-- Adding chunk border sticks outdates multiple chunks.
	t:add_stick(32, 0, 0, 40, 3)
	local x,z = t:get_nearest_chunk_with_outdated_model(0, 0)
	assert(x == 0)
	assert(z == 0)
	local x,z = t:get_nearest_chunk_with_outdated_model(32, 0)
	assert(x == 32)
	assert(z == 0)
	local m = t:build_chunk_model(0, 0)
	assert(type(m) == "table")
	local x,z = t:get_nearest_chunk_with_outdated_model(0, 0)
	assert(x == 32)
	assert(z == 0)
	local m = t:build_chunk_model(32, 0)
	assert(type(m) == "table")
	local x,z = t:get_nearest_chunk_with_outdated_model(0, 0)
	assert(x == nil)
	assert(z == nil)
	-- Clearing chunk models.
	assert(t:clear_chunk_model(0, 0))
	local x,z = t:get_nearest_chunk_with_outdated_model(0, 0)
	assert(x == 0)
	assert(z == 0)
end)

Unittest:add(1, "system", "terrain: filtering", function()
	local Terrain = require("system/terrain")
	local t = Terrain(32, 0.5)
	t:load_chunk(0, 0)
	-- 000011112222
	--   11111111
	-- FFFF
	t:add_stick(0, 0, 4, 4, 1)
	t:add_stick(0, 0, 8, 4, 2)
	t:add_stick_filter_id(0, 0, 2, 8, 1,   0)
	local c = t:get_column(0, 0)
	assert(#c == 3)
	assert(c[1][1] == 0)
	assert(c[1][2] == 2)
	assert(c[2][1] == 1)
	assert(c[2][2] == 6)
	assert(c[3][1] == 2)
	assert(c[3][2] == 4)
	-- 000011112222
	--   33333333
	--     FFFF
	t:clear_column(0, 0)
	t:add_stick(0, 0, 4, 4, 1)
	t:add_stick(0, 0, 8, 4, 2)
	t:add_stick_filter_id(0, 0, 2, 8, 3,   1)
	local c = t:get_column(0, 0)
	assert(#c == 3)
	assert(c[1][1] == 0)
	assert(c[1][2] == 4)
	assert(c[2][1] == 3)
	assert(c[2][2] == 4)
	assert(c[3][1] == 2)
	assert(c[3][2] == 4)
	-- 000011112222
	--   11111111
	--     FFFFFFFF
	t:clear_column(0, 0)
	t:add_stick(0, 0, 4, 4, 1)
	t:add_stick(0, 0, 8, 4, 2)
	t:add_stick_filter_mask(0, 0, 2, 8, 1, 2^1 + 2^2)
	local c = t:get_column(0, 0)
	assert(#c == 3)
	assert(c[1][1] == 0)
	assert(c[1][2] == 4)
	assert(c[2][1] == 1)
	assert(c[2][2] == 6)
	assert(c[3][1] == 2)
	assert(c[3][2] == 2)
	-- 000011112222
	--   33333333
	-- FFFF    FFFF
	t:clear_column(0, 0)
	t:add_stick(0, 0, 4, 4, 1)
	t:add_stick(0, 0, 8, 4, 2)
	t:add_stick_filter_mask(0, 0, 2, 8, 3, 2^0 + 2^2)
	local c = t:get_column(0, 0)
	assert(#c == 5)
	assert(c[1][1] == 0)
	assert(c[1][2] == 2)
	assert(c[2][1] == 3)
	assert(c[2][2] == 2)
	assert(c[3][1] == 1)
	assert(c[3][2] == 4)
	assert(c[4][1] == 3)
	assert(c[4][2] == 2)
	assert(c[5][1] == 2)
	assert(c[5][2] == 2)
end)

Unittest:add(1, "system", "terrain: find nearest empty stick", function()
	local Terrain = require("system/terrain")
	local t = Terrain(32, 0.5)
	t:load_chunk(0, 0)
	t:add_stick(0, 0, 10, 4, 1)
	t:add_stick(0, 0, 20, 7, 1)
	t:add_stick(0, 0, 30, 1, 1)
	-- Initial gap.
	local y = t:find_nearest_empty_stick(0, 0, 5, 10)
	assert(y == 0)
	-- Middle gap.
	local y = t:find_nearest_empty_stick(0, 0, 20, 6)
	assert(y == 14)
	-- Too small gap.
	local y = t:find_nearest_empty_stick(0, 0, 20, 7)
	assert(y == 31)
	-- Top gap.
	local y = t:find_nearest_empty_stick(0, 0, 60, 7)
	assert(y == 31)
end)

Unittest:add(1, "system", "terrain: get stick", function()
	local Terrain = require("system/terrain")
	local t = Terrain(32, 0.5)
	t:load_chunk(0, 0)
	t:add_stick(0, 0, 10, 4, 1)
	t:add_stick(0, 0, 20, 7, 1)
	t:add_stick(0, 0, 30, 1, 1)
	-- First stick.
	local y,h,m = t:get_stick(0, 0, 5)
	assert(y == 0)
	assert(h == 10)
	assert(m == 0)
	-- Middle stick.
	local y,h,m = t:get_stick(0, 0, 25)
	assert(y == 20)
	assert(h == 7)
	assert(m == 1)
	-- Virtual empty stick.
	local y,h,m = t:get_stick(0, 0, 100)
	assert(y == 31)
	assert(h > 10000)
	assert(m == 0)
end)

Unittest:add(1, "system", "terrain: hidden face culling", function()
	local Terrain = require("system/terrain")
	local t = Terrain(32, 0.5)
	-- No culling.
	t:load_chunk(0, 0)
	t:add_stick(5, 3, 0, 10, 1)
	t:add_stick(5, 5, 0, 5, 1)
	local m = t:build_chunk_model(0, 0)
	assert(m:get_vertex_count() == 10 * 6)
	-- Front face.
	t:unload_chunk(0, 0)
	t:load_chunk(0, 0)
	t:add_stick(5, 4, 0, 10, 1)
	t:add_stick(5, 5, 0, 5, 1)
	local m = t:build_chunk_model(0, 0)
	assert(m:get_vertex_count() == 9 * 6)
	-- Back face.
	t:unload_chunk(0, 0)
	t:load_chunk(0, 0)
	t:add_stick(5, 4, 0, 5, 1)
	t:add_stick(5, 5, 0, 10, 1)
	local m = t:build_chunk_model(0, 0)
	assert(m:get_vertex_count() == 9 * 6)
	-- Front and back faces.
	t:unload_chunk(0, 0)
	t:load_chunk(0, 0)
	t:add_stick(5, 5, 0, 5, 1)
	t:add_stick(5, 4, 0, 5, 1)
	local m = t:build_chunk_model(0, 0)
	assert(m:get_vertex_count() == 8 * 6)
	-- Left face.
	t:unload_chunk(0, 0)
	t:load_chunk(0, 0)
	t:add_stick(4, 5, 0, 10, 1)
	t:add_stick(5, 5, 0, 5, 1)
	local m = t:build_chunk_model(0, 0)
	assert(m:get_vertex_count() == 9 * 6)
	-- Right face.
	t:unload_chunk(0, 0)
	t:load_chunk(0, 0)
	t:add_stick(4, 5, 0, 5, 1)
	t:add_stick(5, 5, 0, 10, 1)
	local m = t:build_chunk_model(0, 0)
	assert(m:get_vertex_count() == 9 * 6)
	-- Left and right faces.
	t:unload_chunk(0, 0)
	t:load_chunk(0, 0)
	t:add_stick(4, 5, 0, 5, 1)
	t:add_stick(5, 5, 0, 5, 1)
	local m = t:build_chunk_model(0, 0)
	assert(m:get_vertex_count() == 8 * 6)
	-- Stacked sticks.
	t:unload_chunk(0, 0)
	t:load_chunk(0, 0)
	t:add_stick(4, 5, 0, 1, 1)
	t:add_stick(4, 5, 1, 1, 2)
	t:add_stick(4, 5, 2, 3, 3)
	t:add_stick(5, 5, 0, 2, 1)
	t:add_stick(5, 5, 2, 2, 2)
	t:add_stick(5, 5, 4, 1, 3)
	local m = t:build_chunk_model(0, 0)
	assert(m:get_vertex_count() == 20 * 6)
	-- Sloped sticks.
	--
	-- 10--11--12--13
	--  |   |   |   |
	-- 11--12--13--14
	--  |   |   |   |
	-- 12--13--14--15
	--  |   |   |   |
	-- 13--14--15--16
	t:unload_chunk(0, 0)
	t:load_chunk(0, 0)
	t:add_stick_corners(1, 1,  0, 0, 0, 0,  10, 11, 11, 12,  1)
	t:add_stick_corners(2, 1,  0, 0, 0, 0,  11, 12, 12, 13,  1)
	t:add_stick_corners(3, 1,  0, 0, 0, 0,  12, 13, 13, 14,  1)
	t:add_stick_corners(1, 2,  0, 0, 0, 0,  11, 12, 12, 13,  1)
	t:add_stick_corners(2, 2,  0, 0, 0, 0,  12, 13, 13, 14,  1)
	t:add_stick_corners(3, 2,  0, 0, 0, 0,  13, 14, 14, 15,  1)
	t:add_stick_corners(1, 3,  0, 0, 0, 0,  12, 13, 13, 14,  1)
	t:add_stick_corners(2, 3,  0, 0, 0, 0,  13, 14, 14, 15,  1)
	t:add_stick_corners(3, 3,  0, 0, 0, 0,  14, 15, 15, 16,  1)
	local m = t:build_chunk_model(0, 0)
	assert(m:get_vertex_count() == 21 * 6)
	-- Same as above but with the centermost stick truncated.
	t:add_stick(2, 2, 12, 100, 0)
	local m = t:build_chunk_model(17, 0)
	assert(m:get_vertex_count() == 23 * 6 + 2 * 3)
	-- This is broken?
	--t:add_stick(2, 2, 13, 100, 0)
	--local m = t:build_chunk_model(17, 0)
	--assert(m:get_vertex_count() == 21 * 6 + 4 * 3)
end)

Unittest:add(1, "system", "terrain: internal", function()
	local Terrain = require("system/terrain")
	Los.terrain_unittest()
end)

Unittest:add(1, "system", "terrain: ray cast", function()
	local Terrain = require("system/terrain")
	local Vector = require("system/math/vector")
	local t = Terrain(32, 2)
	t:load_chunk(0, 0)
	t:add_stick(1, 1, 0, 40, 3)
	-- Hit.
	local p,n,gx,gy = t:cast_ray(Vector(-10,1,-10), Vector(10,1,10))
	assert(gx == 1)
	assert(gy == 1)
	assert(math.abs(p.x - 2) < 0.1)
	assert(math.abs(p.y - 1) < 0.1)
	assert(math.abs(p.z - 2) < 0.1)
	assert(n.x < 0.7)
	assert(math.abs(n.y) < 0.1)
	assert(n.z < 0.7)
	-- Miss.
	assert(not t:cast_ray(Vector(-10,1,-10), Vector(-1,1,-1)))
	assert(not t:cast_ray(Vector(5,1,5), Vector(10,1,10)))
	assert(not t:cast_ray(Vector(-13,1,-10), Vector(7,1,10)))
	assert(not t:cast_ray(Vector(-10,40,-10), Vector(10,45,10)))
end)

Unittest:add(1, "system", "terrain: saving and loading", function()
	local Packet = require("system/packet")
	local Terrain = require("system/terrain")
	local t = Terrain(32, 2)
	t:load_chunk(0, 0)
	t:add_stick(1, 1, 0, 40, 3)
	t:add_stick(4, 4, 0, 40, 3)
	t:add_stick(4, 4, 50, 4, 2)
	-- Serialization 1.
	local writer = Packet(1)
	assert(t:get_chunk_data(0, 0, writer))
	-- Deserialization 1.
	local reader = writer:copy_readable()
	assert(t:set_chunk_data(0, 0, reader))
	t:load_chunk(1, 1)
	reader:read()
	assert(t:set_chunk_data(1, 1, reader))
	-- Serialization 2.
	local writer = Packet(1, "uint32", 0, "uint32", 0, "string", "some data")
	assert(t:get_chunk_data(0, 0, writer))
	-- Deserialization 2.
	local reader = writer:copy_readable()
	local ok,x,z,s = reader:read("uint32", "uint32", "string")
	assert(ok)
	assert(t:set_chunk_data(x, z, reader))
end)

Unittest:add(1, "system", "terrain: smooth normals", function()
	local Terrain = require("system/terrain")
	local t = Terrain(32, 0.5)
	t:load_chunk(0, 0)
	t:add_stick_corners(0, 0, 0, 0, 0, 0, 4, 4, 5, 5, 10)
	t:add_stick_corners(0, 1, 0, 0, 0, 0, 5, 5, 5, 5, 11)
	local c = t:get_column(0, 0)
	assert(c[1][1] == 10)
	assert(c[1][2] == 4.5)
	assert(c[1][3] == -0.5)
	assert(c[1][4] == 0)
	assert(c[1][5] == 0)
	assert(c[1][6] == 1)
	assert(c[1][7] == 0)
	assert(c[1][8] == -0.5)
	assert(c[1][9] == 0)
	assert(c[1][10] == 0)
	assert(c[1][11] == 1)
	assert(c[1][12] == 0)
	assert(c[1][13] == 0.5)
	assert(c[1][14] == 0)
	assert(c[1][15] == 0)
	assert(c[1][16] == 1)
	assert(c[1][17] == 0)
	assert(c[1][18] == 0.5)
	assert(c[1][19] == 0)
	assert(c[1][20] == 0)
	assert(c[1][21] == 1)
	assert(c[1][22] == 0)
	t:calculate_smooth_normals(0, 0)
	local c = t:get_column(0, 0)
	assert(c[1][1] == 10)
	assert(c[1][2] == 4.5)
	assert(c[1][3] == -0.5)
	assert(c[1][4] == 0)
	assert(c[1][5] == 0)
	assert(c[1][6] == 1)
	assert(c[1][7] == 0)
	assert(c[1][8] == -0.5)
	assert(c[1][9] == 0)
	assert(c[1][10] == 0)
	assert(c[1][11] == 1)
	assert(c[1][12] == 0)
	assert(c[1][13] == 0.5)
	assert(c[1][14] == 0)
	assert(c[1][15] == 0)
	assert(c[1][16] == 1)
	assert(c[1][17] == 0)
	assert(c[1][18] == 0.5)
	assert(c[1][19] == 1) -- splatted
	assert(math.abs(c[1][20] - 0) < 0.1) -- smoothed
	assert(math.abs(c[1][21] - 0.924) < 0.1) -- smoothed
	assert(math.abs(c[1][22] + 0.383) < 0.1) -- smoothed
	local c = t:get_column(0, 1)
	assert(c[1][1] == 11)
	assert(c[1][2] == 5)
	assert(c[1][3] == 0)
	assert(c[1][4] == 0)
	assert(c[1][5] == 0)
	assert(c[1][6] == 1)
	assert(c[1][7] == 0)
	assert(c[1][8] == 0)
	assert(c[1][9] == 1) -- splatted
	assert(math.abs(c[1][10] - 0) < 0.1) -- smoothed
	assert(math.abs(c[1][11] - 0.924) < 0.1) -- smoothed
	assert(math.abs(c[1][12] + 0.383) < 0.1) -- smoothed
	assert(c[1][13] == 0)
	assert(c[1][14] == 0)
	assert(c[1][15] == 0)
	assert(c[1][16] == 1)
	assert(c[1][17] == 0)
	assert(c[1][18] == 0)
	assert(c[1][19] == 0)
	assert(c[1][20] == 0)
	assert(c[1][21] == 1)
	assert(c[1][22] == 0)
end)

Unittest:add(1, "system", "terrain: sphere iterator", function()
	local Terrain = require("system/terrain")
	local Vector = require("system/math/vector")
	local t = Terrain(32, 0.5)
	for x,z,y,a,b,c,d in t:get_sticks_in_sphere(Vector(5,6,7), 1) do
		assert(x >= 8)
		assert(x <= 12)
		assert(z >= 12)
		assert(z <= 16)
		assert(y == 6)
		assert(a >= 0 and b >= 0 and c >= 0 and d >= 0)
		assert(a > 0 or b > 0 or c > 0 or d > 0)
		assert(a <= 1 and b <= 1 and c <= 1 and d <= 1)
	end
end)
