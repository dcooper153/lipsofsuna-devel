local File = require("system/file")

Ui:add_state{
	state = "loadgame",
	root = "mainmenu",
	label = "Load game",
	background = function()
		return Widgets.Uibackground("mainmenu1")
	end}

Ui:add_widget{
	state = "loadgame",
	widget = function()
		local widgets = {}
		for k,v in ipairs(File:scan_save_directory("")) do
			local file = string.match(v, "save([a-z0-9_]*).sqlite")
			if file then
				local name = Operators.single_player:decode_save_filename(file)
				table.insert(widgets, Widgets.Uibutton(name, function()
					Settings.file = file
					Settings.generate = false
					Main:start_game("Normal")
				end))
			end
		end
		return widgets
	end}
