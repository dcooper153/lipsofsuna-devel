require "client/shader"
require "client/target"
require "client/theme"
require "client/shaders/adamantium"
require "client/shaders/default"
require "client/shaders/diffnormspec"
require "client/shaders/eye"
require "client/shaders/glass"
require "client/shaders/foliage"
require "client/shaders/fur"
require "client/shaders/hair"
require "client/shaders/luminous"
require "client/shaders/normalmap"
require "client/shaders/nolitdiff"
require "client/shaders/particle"
require "client/shaders/postprocess"
require "client/shaders/skin"
require "client/shaders/terrain"
require "client/shaders/widget"
require "client/widgets/background"
require "client/widgets/scene"

local oldsetter = Object.setter
Object.setter = function(self, key, value)
	if key == "spec" then
		oldsetter(self, key, value)
		self.model = value.model
		if value.models then
			local m = self.model:copy()
			for k,v in pairs(value.models) do
				if k ~= "skeleton" then
					m:merge(Model:load{file = v})
				end
			end
			m:calculate_bounds()
			m:changed()
			self.model = m
		end
		if value.type == "species" then
			self:animate{animation = "idle", channel = 1, permanent = true}
			self:update_animations{secs = 0}
			self:deform_mesh()
		end
	else
		oldsetter(self, key, value)
	end
end

for k,s in pairs(Shader.dict_name) do
	s:set_quality(2)
end

Editor = Class()

Editor.new = function(clss)
	local pattern = Pattern:find{name = Settings.pattern}
	if not pattern then return end
	local self = Class.new(clss)
	self.prev_tiles = {}
	-- Load the pattern.
	self.pattern = pattern
	self.origin = Vector(100,100,100)
	self.size = pattern.size or Vector(10,10,10)
	Voxel:place_pattern{point = self.origin, name = Settings.pattern}
	-- Camera and lighting.
	self.camera = Camera{far = 40.0, fov = 1.1, mode = "first-person", near = 0.01, position_smoothing = 0.15, rotation_smoothing = 0.15}
	self.camera.target_position = (self.origin + Vector(0,2,-5)) * Voxel.tile_size
	self.camera.target_rotation = Quaternion(0, 1, 0, 0)
	self.camera:warp()
	self.light = Light{ambient = {0.3,0.3,0.3,1.0}, diffuse={0.6,0.6,0.6,1.0}, equation={1.0,0.0,0.01}, priority = 2}
	self.light.enabled = true
	-- Item selector.
	local items = {}
	for k in pairs(Itemspec.dict_name) do table.insert(items, k) end
	table.sort(items)
	self.combo_items = Widgets.ComboBox(items)
	self.combo_items:activate{index = 1, pressed = false}
	self.button_items = Widgets.Button{text = "Add", pressed = function() self.mode = "add item" end}
	self.group_items = Widget{cols = 2, rows = 1}
	self.group_items:set_expand{col = 1}
	self.group_items:set_child{col = 1, row = 1, widget = self.combo_items}
	self.group_items:set_child{col = 2, row = 1, widget = self.button_items}
	-- Obstacle selector.
	local obstacles = {}
	for k in pairs(Obstaclespec.dict_name) do table.insert(obstacles, k) end
	table.sort(obstacles)
	self.combo_obstacles = Widgets.ComboBox(obstacles)
	self.combo_obstacles:activate{index = 1, pressed = false}
	self.button_obstacles = Widgets.Button{text = "Add", pressed = function() self.mode = "add obstacle" end}
	self.group_obstacles = Widget{cols = 2, rows = 1}
	self.group_obstacles:set_expand{col = 1}
	self.group_obstacles:set_child{col = 1, row = 1, widget = self.combo_obstacles}
	self.group_obstacles:set_child{col = 2, row = 1, widget = self.button_obstacles}
	-- Species selector.
	local species = {}
	for k in pairs(Species.dict_name) do table.insert(species, k) end
	table.sort(species)
	self.combo_species = Widgets.ComboBox(species)
	self.combo_species:activate{index = 1, pressed = false}
	self.button_species = Widgets.Button{text = "Add", pressed = function() self.mode = "add species" end}
	self.group_species = Widget{cols = 2, rows = 1}
	self.group_species:set_expand{col = 1}
	self.group_species:set_child{col = 1, row = 1, widget = self.combo_species}
	self.group_species:set_child{col = 2, row = 1, widget = self.button_species}
	-- Tile selector.
	local tiles = {}
	for k in pairs(Material.dict_name) do table.insert(tiles, k) end
	table.sort(tiles)
	self.combo_tiles = Widgets.ComboBox(tiles)
	self.combo_tiles:activate{index = 1, pressed = false}
	self.button_tiles = Widgets.Button{text = "Add", pressed = function() self.mode = "add tile" end}
	self.group_tiles = Widget{cols = 2, rows = 1}
	self.group_tiles:set_expand{col = 1}
	self.group_tiles:set_child{col = 1, row = 1, widget = self.combo_tiles}
	self.group_tiles:set_child{col = 2, row = 1, widget = self.button_tiles}
	-- Buttons.
	self.button_delete = Widgets.Button{text = "Delete", pressed = function() self.mode = "delete" end}
	self.button_save = Widgets.Button{text = "Save", pressed = function() self:save() end}
	-- Packing.
	self.group = Widgets.Frame{cols = 1, rows = 6}
	self.group:set_expand{col = 1}
	self.group:set_child{col = 1, row = 1, widget = self.group_items}
	self.group:set_child{col = 1, row = 2, widget = self.group_obstacles}
	self.group:set_child{col = 1, row = 3, widget = self.group_species}
	self.group:set_child{col = 1, row = 4, widget = self.group_tiles}
	self.group:set_child{col = 1, row = 5, widget = self.button_delete}
	self.group:set_child{col = 1, row = 6, widget = self.button_save}
	self.scene = Widgets.Scene{cols = 2, rows = 2, camera = self.camera, margins = {5,5,0,0}, behind = true, floating = true, fullscreen = true}
	self.scene.pressed = function(w, args) self:pressed(args) end
	self.scene:set_expand{col = 2, row = 2}
	self.scene:set_child{col = 1, row = 1, widget = self.group}
	-- Corner markers.
	self.corners = {}
	for i=1,8 do
		self.corners[i] = Object{model = "tree1", realized = true}
	end
	self:update_corners()
	return self
end

Editor.fill = function(self, p1, p2, erase)
	-- Find the fill material.
	local mat = 0
	if not erase then
		local m = Material:find{name = self.combo_tiles.text}
		if m then mat = m.id end
	end
	-- Fix the order of the range.
	local x1,x2 = p1.x,p2.x
	local y1,y2 = p1.y,p2.y
	local z1,z2 = p1.z,p2.z
	if x1 > x2 then x1,x2 = x2,x1 end
	if y1 > y2 then y1,y2 = y2,y1 end
	if z1 > z2 then z1,z2 = z2,z1 end
	-- Fill all voxels within the range.
	for x = x1,x2 do
		for y = y1,y2 do
			for z = z1,z2 do
				Voxel:set_tile(Vector(x,y,z), mat)
			end
		end
	end
end

Editor.pressed = function(self, args)
	local point,object,tile = Target:pick_ray{camera = self.camera}
	if not point then return end
	if args.button ~= 1 then return end
	if self.mode == "add item" then
		local spec = Itemspec:find{name = self.combo_items.text}
		Object{position = point, realized = true, spec = spec}
	elseif self.mode == "add obstacle" then
		local spec = Obstaclespec:find{name = self.combo_obstacles.text}
		Object{position = point, realized = true, spec = spec}
	elseif self.mode == "add species" then
		local spec = Species:find{name = self.combo_species.text}
		Object{position = point, realized = true, spec = spec}
	elseif self.mode == "add tile" then
		local mat = Material:find{name = self.combo_tiles.text}
		local t,p = Voxel:find_tile{match = "empty", point = point}
		if p then
			Voxel:set_tile{point = p, tile = mat.id}
			self.prev_tiles[2] = self.prev_tiles[1]
			self.prev_tiles[1] = p
		end
	elseif self.mode == "delete" then
		if object then
			if object.spec then
				object.realized = false
			end
		elseif tile then
			Voxel:set_tile{point = tile, tile = 0}
		end
	end
end

Editor.save = function(self)
	local items = {}
	local obstacles = {}
	local species = {}
	-- Collect objects.
	for k,v in pairs(Object.objects) do
		if v.realized and v.spec then
			if v.spec.type == "item" then
				table.insert(items, v)
			elseif v.spec.type == "obstacle" then
				table.insert(obstacles, v)
			elseif v.spec.type == "species" then
				table.insert(species, v)
			end
		end
	end
	-- Sort objects.
	-- Predictable order leads to cleaner patches.
	local roundvec = function(v)
		local p = v * Voxel.tile_scale - self.origin
		return {math.floor(p.x * 100) * 0.01,
		        math.floor(p.y * 100) * 0.01,
		        math.floor(p.z * 100) * 0.01}
	end
	local sortobj = function(a, b)
		local ap = roundvec(a.position)
		local bp = roundvec(b.position)
		if ap[1] < bp[1] then return true end
		if ap[1] > bp[1] then return false end
		if ap[2] < bp[2] then return true end
		if ap[2] > bp[2] then return false end
		if ap[3] < bp[3] then return true end
		if ap[3] > bp[3] then return false end
		if a.spec.name < a.spec.name then return true end
		return false
	end
	table.sort(items, sortobj)
	table.sort(obstacles, sortobj)
	table.sort(species, sortobj)
	-- Format categories.
	local t = "Pattern{\n\tname = \"" .. self.pattern.name .. "\",\n\tsize = " .. tostring(self.size) .. ",\n"
	if self.pattern.categories then
		local categories = {}
		for k,v in pairs(self.pattern.categories) do
			table.insert(categories, k)
		end
		table.sort(categories)
		t = t .. "\tcategories = {"
		local comma
		for k,v in ipairs(categories) do
			if comma then t = t .. "," end
			t = t .. "\"" .. v .. "\""
			comma = true
		end
		t = t .. "},\n"
	end
	-- Format objects.
	local addobj = function(t, k, v)
		local p = roundvec(v.position)
		if k > 1 then t = t .. ",\n" end
		return t .. "\t\t{" .. p[1] .. "," .. p[2] .. "," .. p[3] .. ",\"" .. v.spec.name .. "\"}"
	end
	local comma
	if #items > 0 then
		t = t .. "\titems = {\n"
		for k,v in ipairs(items) do t = addobj(t, k, v) end
		t = t .. "}"
		comma = true
	end
	if #obstacles > 0 then
		if comma then t = t .. ",\n" end
		t = t .. "\tobstacles = {\n"
		for k,v in ipairs(obstacles) do t = addobj(t, k, v) end
		t = t .. "}"
		comma = true
	end
	if #species > 0 then
		if comma then t = t .. ",\n" end
		t = t .. "\tcreatures = {\n"
		for k,v in ipairs(species) do t = addobj(t, k, v) end
		t = t .. "}"
		comma = true
	end
	-- Format tiles.
	local newline
	local first = true
	for z = 0,self.size.z-1 do
		for y = 0,self.size.y-1 do
			newline = true
			for x = 0,self.size.x-1 do
				local v = Voxel:get_tile{point = self.origin + Vector(x, y, z)}
				if v ~= 0 then
					local mat = Material:find{id = v}
					if mat then
						if first then
							if comma then t = t .. ",\n" end
							t = t .. "\ttiles = {\n\t\t"
							first = nil
							newline = nil
						elseif newline then
							t = t .. ",\n\t\t"
							newline = nil
						else
							t = t .. ", "
						end
						t = t .. "{" .. x .. "," .. y .. "," .. z .. ",\"" .. mat.name .. "\"}"
					end
				end
			end
		end
	end
	if not first then
		t = t .. "}"
	end
	t = t .. "}\n"
	-- Print to the console.
	print(t)
end

Editor.update_corners = function(self)
	local s = self.size
	local p = {Vector(0,0,0), Vector(s.x,0,0), Vector(0,s.y,0), Vector(s.x,s.y,s.z),
		Vector(0,0,s.z), Vector(s.x,0,s.z), Vector(0,s.y,s.z), Vector(s.x,s.y,s.z)}
	for i=1,8 do
		self.corners[i].position = (self.origin + p[i]) * Voxel.tile_size
	end
end

------------------------------------------------------------------------------

Keys = {ESCAPE = 27, SPACE = 32, COMMA = 44,
	a = 97, c = 99, d = 100, e = 101, f = 102, i = 105, k = 107, o = 111, q = 113, y = 121, z = 122,
	x = 120, w = 119, r = 114, s = 115, F1 = 282, F2 = 283, F3 = 284, F4 = 285, F5 = 286,
	F6 = 287, F7 = 288, F8 = 289, F9 = 290, F10 = 291, F11 = 292, F12 = 293,
	LSHIFT = 304, RCTRL = 305, LCTRL = 306, PRINT = 316}
Eventhandler{type = "keypress", func = function(self, args)
	if not Widgets:handle_event(args) then
		local c = Editor.inst.camera
		if args.code == Keys.w then
			-- Forward.
			c.target_position = c.target_position + c.rotation * Vector(0,0,-1)
		elseif args.code == Keys.s then
			-- Backward.
			c.target_position = c.target_position + c.rotation * Vector(0,0,1)
		elseif args.code == Keys.a then
			-- Turn left.
			c.target_rotation = c.rotation * Quaternion{axis = Vector(0,1), angle = 0.3}
		elseif args.code == Keys.d then
			-- Turn right.
			c.target_rotation = c.rotation * Quaternion{axis = Vector(0,1), angle = -0.3}
		elseif args.code == Keys.q then
			-- Up.
			c.target_position = c.target_position + c.rotation * Vector(0,1,0)
		elseif args.code == Keys.z then
			-- Down.
			c.target_position = c.target_position + c.rotation * Vector(0,-1,0)
		elseif args.code == Keys.e then
			-- Left.
			c.target_position = c.target_position + c.rotation * Vector(-1,0,0)
		elseif args.code == Keys.r then
			-- Right.
			c.target_position = c.target_position + c.rotation * Vector(1,0,0)
		elseif args.code == Keys.f then
			-- Fill.
			if Editor.inst.prev_tiles[1] and Editor.inst.prev_tiles[2] then
				Editor.inst:fill(Editor.inst.prev_tiles[1], Editor.inst.prev_tiles[2])
			end
		elseif args.code == Keys.k then
			-- Erase.
			if Editor.inst.prev_tiles[1] and Editor.inst.prev_tiles[2] then
				Editor.inst:fill(Editor.inst.prev_tiles[1], Editor.inst.prev_tiles[2], true)
			end
		elseif args.code == Keys.F1 then
			Editor.inst:save()
		elseif args.code == Keys.PRINT then
			Client:screenshot()
		end
	end
end}

Eventhandler{type = "keyrelease", func = function(self, args)
end}

Eventhandler{type = "mousepress", func = function(self, args)
	Widgets:handle_event(args)
end}

Eventhandler{type = "quit", func = function(self, args)
	Program.quit = true
end}

Eventhandler{type = "tick", func = function(self, args)
	Editor.inst.camera:update(args.secs)
	Editor.inst.light.position = Editor.inst.camera.target_position +
		Editor.inst.camera.rotation * Vector(0,0,-5)
	-- Update the cursor.
	Widgets.Cursor.inst:update()
end}

-- Initialize the UI state.
Widgets.Cursor.inst = Widgets.Cursor(Iconspec:find{name = "cursor1"})
Editor.inst = Editor()
if not Editor.inst then
	Program.quit = true
end

-- Main loop.
while not Program.quit do
	-- Update program state.
	Program:update()
	-- Handle events.
	local event = Program:pop_event()
	while event do
		Eventhandler:event(event)
		event = Program:pop_event()
	end
	-- Render the scene.
	Client:clear_buffer()
	Widgets:draw()
	Client:swap_buffers()
	-- Focus widgets.
	Widgets:update()
end
