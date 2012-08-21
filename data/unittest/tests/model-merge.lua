Unittest:add(1, "system", "model merger", function()
	local ModelMerger = require("system/model-merger")
	-- Creating the merger.
	local m = ModelMerger()
	assert(m)
	assert(m.handle)
	-- Finishing.
	assert(not m:pop_model())
	m:finish()
	repeat until m:pop_model()
	-- TODO: Merging models.
end)
