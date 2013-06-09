Unittest:add(1, "lipsofsuna", "model manager", function()
	local ModelManager = require("main/model-manager")
	-- Initialization.
	local mgr = ModelManager()
	assert(tostring(mgr) == "data:ModelManager")
	for k,v in pairs(mgr.models_by_name) do
		assert(false)
	end
	-- Model loading.
	local tmp = setmetatable({}, {__mode = "kv"})
	tmp.a1 = mgr:find_by_name("test1")
	tmp.a2 = mgr:find_by_name("test2")
	tmp.b1 = mgr:find_by_name("test1")
	tmp.b2 = mgr:find_by_name("test2")
	assert(tmp.a1)
	assert(tmp.a1 == tmp.b1)
	assert(tmp.a1 ~= tmp.a2)
	assert(tmp.a2)
	assert(tmp.a2 == tmp.b2)
	-- Preservation of unused models.
	collectgarbage()
	assert(tmp.a1)
	assert(tmp.b1)
	mgr:update(0.1)
	collectgarbage()
	assert(tmp.a1)
	assert(tmp.b1)
	-- Removal of unused models.
	mgr:update(100)
	collectgarbage()
	assert(not tmp.a1)
	assert(not tmp.b1)
	-- Preservation of used models.
	local kept1 = mgr:find_by_name("test1")
	mgr:update(100)
	collectgarbage()
	local kept2 = mgr:find_by_name("test1")
	assert(kept1 == kept2)
	-- Refreshing of reused models.
	mgr:update(100)
	assert(not mgr.seconds_by_model[kept1])
	local kept3 = mgr:find_by_name("test1")
	assert(mgr.seconds_by_model[kept1])
end)
