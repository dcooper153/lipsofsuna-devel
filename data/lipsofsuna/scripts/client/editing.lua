Editing = Class()
Editing.visible = false

--- Initializes the editor.
-- @param clss Editor class.
Editing.init = function(clss)

	-- Creating objects.
	clss.popup_class_button = Widgets.ComboBox{
		"creature",
		"item",
		"item-cat",
		"obstacle",
		"obstacle-cat",
		-- FIXME:
		"door",
		"chest",
		"mover",
		"object",
		"spawner"}
	clss.popup_class_button:activate{index = 2}
	clss.entry_model = Entry()
	clss.group_object_create = Group{cols = 1}
	clss.group_object_create:set_expand{col = 1}
	clss.group_object_create:append_row(Button{style = "label", text = "Type:"})
	clss.group_object_create:append_row(clss.popup_class_button)
	clss.group_object_create:append_row(Button{style = "label", text = "Model:"})
	clss.group_object_create:append_row(clss.entry_model)
	clss.group_object_create:append_row(Button{text = "Create", pressed = function()
		Network:send{packet = Packet(packets.ADMIN_SPAWN,
			"string", Editing.popup_class_button.text,
			"string", Editing.entry_model.text)}
	end})

	-- Deleting objects.
	clss.group_object_delete = Group{cols = 1}
	clss.group_object_delete:set_expand{col = 1}
	clss.group_object_delete:append_row(Button{text = "Delete", pressed = function()
		local packet = Packet(packets.ADMIN_DELETE)
		for k,v in pairs(Object.selected_objects) do
			packet:write("uint32", v.id)
		end
		Network:send{packet = packet}
	end})

	-- Creating terrain.
	clss.preview_terrain = Group{style = "button"}
	clss.preview_terrain:set_request{width = 200, height=200}
	clss.preview_terrain.scene = Scene()
	clss.preview_terrain.render = function(self)
		--[[Player.camera.viewport = {self.x, self.y, self.width, self.height}
		self.scene:draw_begin{
			modelview = Player.camera.modelview,
			projection = Player.camera.projection,
			viewport = Player.camera.viewport}
		self.scene:draw_deferred_begin()
		self.scene:draw_deferred_opaque()
		self.scene:draw_deferred_end()
		self.scene:draw_forward_transparent()
		self.scene:draw_post_process{shader = "postprocess-vert-hdr"}
		self.scene:draw_post_process{shader = "postprocess-horz-hdr"}
		self.scene:draw_end()--]]
	end
	clss.spin_terrain = Widgets.ComboBox()
	local mats = {}
	for k,v in ipairs(Material.dict_id) do table.insert(mats, v.name) end
	table.sort(mats)
	for k,v in ipairs(mats) do
		clss.spin_terrain:append{text = v}
	end
	clss.spin_terrain:activate{index = 1}
	clss.group_terrain_create = Group{cols = 1}
	clss.group_terrain_create:set_expand{col = 1}
	clss.group_terrain_create:append_row(Button{style = "label", text = "Material:"})
	clss.group_terrain_create:append_row(clss.spin_terrain)
	clss.group_terrain_create:append_row(clss.preview_terrain)
	clss.group_terrain_create:append_row(Button{text = "Create", pressed = function()
		Editing:insert()
	end})

	-- Deleting terrain.
	clss.group_terrain_delete = Group{cols = 1}
	clss.group_terrain_delete:set_expand{col = 1}
	clss.group_terrain_delete:append_row(Button{text = "Delete", pressed = function()
		Editing:erase()
	end})

	-- Rotating terrain.
	clss.spin_rot = Widgets.ComboBox{"x", "y", "z"}
	clss.spin_rot:activate{index = 2}
	clss.group_terrain_rotate = Group{cols = 1}
	clss.group_terrain_rotate:append_row(Button{style = "label", text = "Axis:"})
	clss.group_terrain_rotate:set_expand{col = 1}
	clss.group_terrain_rotate:append_row(clss.spin_rot)
	clss.group_terrain_rotate:append_row(Button{text = "Rotate", pressed = function()
		Editing:rotate()
	end})

	-- Popup button.
	clss.popup_button = Widgets.ComboBox{
		{"Objects: create", function() Editing:set_mode(1) end},
		{"Objects: delete", function() Editing:set_mode(2) end},
		{"Terrain: create", function() Editing:set_mode(3) end},
		{"Terrain: delete", function() Editing:set_mode(4) end},
		{"Terrain: rotate", function() Editing:set_mode(5) end}}

	-- Packing.
	clss.dialog = Group{cols = 1, rows = 2}
	clss.dialog.spacings = {0, 20}
	clss.dialog:set_expand{col = 1}
	clss.dialog:set_request{width = 200, height = 300}
	clss.dialog:set_child{col = 1, row = 1, widget = clss.popup_button}
	clss:set_mode(1)
end

--- Sets the mode of the editor dialog.
-- @param clss Editing class.
-- @param mode Mode number.
Editing.set_mode = function(clss, mode)
	local funs =
	{
		function()
			clss.dialog:set_child{col = 1, row = 2, widget = clss.group_object_create}
			clss.popup_button.text = "Object: create"
		end,
		function()
			clss.dialog:set_child{col = 1, row = 2, widget = clss.group_object_delete}
			clss.popup_button.text = "Object: delete"
		end,
		function()
			clss.dialog:set_child{col = 1, row = 2, widget = clss.group_terrain_create}
			clss.popup_button.text = "Terrain: create"
		end,
		function()
			clss.dialog:set_child{col = 1, row = 2, widget = clss.group_terrain_delete}
			clss.popup_button.text = "Terrain: delete"
		end,
		function()
			clss.dialog:set_child{col = 1, row = 2, widget = clss.group_terrain_rotate}
			clss.popup_button.text = "Terrain: rotate"
		end
	}
	local fun = funs[mode]
	fun()
end

--- Toggles visibility of the editor.
-- @param clss Editor class.
function Editing.toggle(clss)
	clss.visible = not clss.visible
	Gui.editor_group.visible = clss.visible
end

function Editing.erase(self)
	local function func(where, id, slot)
		if where == "map" and slot ~= nil then
			local t,p = Voxel:find_tile{match = "full", point = slot}
			if p then
				t.terrain = 0
				t.damage = 0
				t.rotation = 0
				Network:send{packet = Packet(packets.EDIT_TILE,
					"int32", p.x, "int32", p.y, "int32", p.z,
					"uint8", t.terrain, "uint8", t.damage, "uint8", t.rotation)}
			end
			Editing:erase()
		end
	end
	Target:start("Erasing terrain...", func)
end

function Editing.insert(self)
	local function func(where, id, slot)
		if where == "map" and slot ~= nil then
			local t,p = Voxel:find_tile{match = "empty", point = slot}
			if p then
				t.terrain = Material:find{name = self.spin_terrain.text}.id
				t.damage = 0
				t.rotation = 0
				Network:send{packet = Packet(packets.EDIT_TILE,
					"uint32", p.x, "uint32", p.y, "uint32", p.z,
					"uint8", t.terrain, "uint8", t.damage, "uint8", t.rotation)}
			end
			Editing:insert(how)
		end
	end
	Target:start("Creating terrain...", func)
end

function Editing.rotate(self)
	local function func(where, id, slot)
		if where == "map" and slot ~= nil then
			local t,p = Voxel:find_tile{match = "full", point = slot}
			if p then
				t:rotate{axis = self.spin_rot.value}
				Network:send{packet = Packet(packets.EDIT_TILE,
					"int32", p.x, "int32", p.y, "int32", p.z,
					"uint8", t.terrain, "uint8", t.damage, "uint8", t.rotation)}
			end
			Editing:rotate()
		end
	end
	Target:start("Rotating terrain...", func)
end

Editing:init()
