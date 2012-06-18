require "system/model-merge"

Unittest:add("model merger", function()
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
