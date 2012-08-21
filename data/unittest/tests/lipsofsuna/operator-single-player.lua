Unittest:add(1, "lipsofsuna", "operator single player", function()
	Operators = {}
	require "core/client/operators/single-player"
	-- Filename encoding.
	Operators.single_player:set_save_name("Test file")
	local res = Operators.single_player:get_save_filename()
	assert(res == "_54est_20file")
	-- Filename decoding.
	Operators.single_player:set_save_filename("_3C_44erp_3E")
	local res = Operators.single_player:get_save_name()
	assert(res == "<Derp>")
end)
