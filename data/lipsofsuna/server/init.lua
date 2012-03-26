if Settings and Settings.server then
	require "system/password"
	require "system/vision"
	require "common/inventory"
	require "common/skills"
	require "common/stats"
	Object.load_meshes = false
	Physics.enable_simulation = true
end
