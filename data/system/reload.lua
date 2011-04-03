if not Los.program_load_extension("reload") then
	error("loading extension `reload' failed")
end

Reload = Class()
Reload.class_name = "Reload"

Reload:add_getters{
	function(s) return Los.reload_get_enabled() end}

Reload:add_setters{
	function(s, v) Los.reload_set_enabled(v) end}
