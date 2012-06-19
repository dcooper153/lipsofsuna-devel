Unittest:add(1, "model merger", function()
	require "system/model-merge"
	-- Creating the merger.
	local m = Merger()
	assert(m)
	assert(m.handle)
	-- Finishing.
	assert(not m:pop_model())
	m:finish()
	repeat until m:pop_model()
	-- TODO: Merging models.
end)
