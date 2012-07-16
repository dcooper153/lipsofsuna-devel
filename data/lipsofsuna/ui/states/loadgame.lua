Ui:add_state{
	state = "loadgame",
	root = "mainmenu",
	label = "Load game",
	grab = function()
		return false
	end,
	background = function()
		return Widgets.Background{fullscreen = true, image = "mainmenu1"}
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
					Client:start_single_player()
				end))
			end
		end
		return widgets
	end}
