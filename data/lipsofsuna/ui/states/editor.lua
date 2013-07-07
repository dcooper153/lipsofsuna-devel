Ui:add_state{
	state = "editor",
	root = "editor",
	hint = "",
	exit_root = function()
		Client.editor:reset()
	end,
	init = function()
		Client.effects:switch_music_track("game")
		if not Client.editor.initialized then
			Client.editor:initialize()
		end
	end,
	update = function(secs)
		if not Client.editor.pattern then
			Ui:set_state("editor/menu")
		else
			Client.editor:update(secs)
		end
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "editor/menu",
	label = "Menu",
	update = function(secs)
		Client.editor:update(secs)
	end}

Ui:add_widget{
	state = "editor/menu",
	widget = function() return Widgets.Uitransition("Load", "editor/load") end}

Ui:add_widget{
	state = "editor/menu",
	widget = function() return Widgets.Uitransition("Add", "editor/add") end}

Ui:add_widget{
	state = "editor/menu",
	widget = function()
		return Widgets.Uibutton("Delete", function()
			Client.editor:delete()
		end)
	end}

Ui:add_widget{
	state = "editor/menu",
	widget = function()
		return Widgets.Uibutton("Save", function()
			-- TODO: Path selection
			Client.editor:save()
		end)
	end}

Ui:add_widget{
	state = "editor/menu",
	widget = function() return Widgets.Uitransition("Introspect", "introspect") end}

Ui:add_widget{
	state = "editor/menu",
	widget = function() return Widgets.Uitransition("Quit", "quit") end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "editor/load",
	label = "Load map",
	update = function(secs) Client.editor:update(secs) end}

Ui:add_state{
	state = "editor/load",
	init = function()
		local widgets = {}
			-- Get the list of map names.
			local widgets = {}
			for k in pairs(Patternspec.dict_name) do table.insert(widgets, k) end
			table.sort(widgets)
			-- Create widgets for the maps.
			for k,v in pairs(widgets) do
				widgets[k] = Widgets.Uibutton(v, function()
					Client.editor:load(v)
				end)
			end
			return widgets
		end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "editor/add",
	label = "Add objects",
	update = function(secs) Client.editor:update(secs) end}

Ui:add_widget{
	state = "editor/add",
	widget = function() return Widgets.Uitransition("Items", "editor/items") end}

Ui:add_widget{
	state = "editor/add",
	widget = function() return Widgets.Uitransition("Obstacles", "editor/obstacles") end}

Ui:add_widget{
	state = "editor/add",
	widget = function() return Widgets.Uitransition("Static objects", "editor/statics") end}

Ui:add_widget{
	state = "editor/add",
	widget = function() return Widgets.Uitransition("Actors", "editor/actors") end}

Ui:add_widget{
	state = "editor/add",
	widget = function() return Widgets.Uitransition("Tiles", "editor/tiles") end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "editor/items",
	label = "Create items",
	update = function(secs) Client.editor:update(secs) end}

Ui:add_state{
	state = "editor/items",
	init = function()
		-- Get the list of item names.
		local widgets = {}
		for k in pairs(Itemspec.dict_name) do table.insert(widgets, k) end
		table.sort(widgets)
		-- Create widgets for the items.
		for k,v in pairs(widgets) do
			widgets[k] = Widgets.Uieditoritem(v)
		end
		return widgets
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "editor/obstacles",
	label = "Create obstacles",
	update = function(secs) Client.editor:update(secs) end}

Ui:add_state{
	state = "editor/obstacles",
	init = function()
		-- Get the list of obstacle names.
		local widgets = {}
		for k in pairs(Obstaclespec.dict_name) do table.insert(widgets, k) end
		table.sort(widgets)
		-- Create widgets for the obstacles.
		for k,v in pairs(widgets) do
			widgets[k] = Widgets.Uibutton(v, function()
				Client.editor:create_obstacle(v)
			end)
		end
		return widgets
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "editor/statics",
	label = "Create static objects",
	update = function(secs) Client.editor:update(secs) end}

Ui:add_state{
	state = "editor/statics",
	init = function()
		-- Get the list of static object names.
		local widgets = {}
		for k in pairs(Staticspec.dict_name) do table.insert(widgets, k) end
		table.sort(widgets)
		-- Create widgets for the static objects.
		for k,v in pairs(widgets) do
			widgets[k] = Widgets.Uibutton(v, function()
				Client.editor:create_static(v)
			end)
		end
		return widgets
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "editor/actors",
	label = "Create actors",
	update = function(secs) Client.editor:update(secs) end}

Ui:add_state{
	state = "editor/actors",
	init = function()
		-- Get the list of actor names.
		local widgets = {}
		for k in pairs(Actorspec.dict_name) do table.insert(widgets, k) end
		table.sort(widgets)
		-- Create widgets for the actors.
		for k,v in pairs(widgets) do
			widgets[k] = Widgets.Uibutton(v, function()
				Client.editor:create_actor(v)
			end)
		end
		return widgets
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "editor/tiles",
	label = "Create voxels",
	update = function(secs) Client.editor:update(secs) end}

Ui:add_state{
	state = "editor/tiles",
	init = function()
		-- Get the list of tile names.
		local widgets = {}
		for k in pairs(Material.dict_name) do table.insert(widgets, k) end
		table.sort(widgets)
		-- Create widgets for the tiles.
		for k,v in pairs(widgets) do
			widgets[k] = Widgets.Uibutton(v, function()
				Client.editor:create_tile(v)
			end)
		end
		return widgets
	end}
