Speedline = Class()

--- Creates a new speed line effect for the object.
-- @param clss Speedline class.
-- @param args Arguments.<ul>
--   <li>delay: Starting delay in seconds.</li>
--   <li>duration: Lifetime in seconds.</li>
--   <li>length: Length in vertices.</li>
--   <li>object: Object.</li></ul>
-- @return Speedline effect.
Speedline.new = function(clss, args)
	self = Class.new(clss)
	self.object = args.object
	self.path = {}
	self.verts = {}
	self.timer = 0
	self.length = args.length or 10
	self.delay = args.delay or 0
	self.duration = args.duration or 10000
	self.model = Model()
	self.model:add_material{material = "speedline1"}
	self.render = RenderObject()
	Game.scene_nodes_by_ref[self] = true
	return self
end

Speedline.detach = function(self)
	self.render:set_visible(false)
	if self.object then
		self.object.speedline = nil
	end
	Game.scene_nodes_by_ref[self] = nil
end

Speedline.add_slice = function(self)
	local l = self.length
	local h = 0.07
	local n = #self.path
	-- Get the current line vertices.
	local p1 = self.object:find_node{name = "#blade1"}
	local p2 = self.object:find_node{name = "#blade2"}
	if not p1 or not p2 then
		p1 = self.object:get_position():copy():subtract_xyz(0,h,0)
		p2 = self.object:get_position():copy():add_xyz(0,h,0)
	else
		p1 = self.object:get_position():copy():add(self.object:get_rotation() * p1)
		p2 = self.object:get_position():copy():add(self.object:get_rotation() * p2)
	end
	p1:subtract(self.position)
	p2:subtract(self.position)
	-- Extrude from the previous vertices.
	table.insert(self.path, {p1,p2})
	if n > 1 then
		local a = self.verts[6 * (n - 1) - 4]
		local b = {p1.x, p1.y, p1.z, 1}
		local c = {p2.x, p2.y, p2.z, 1}
		local d = self.verts[6 * (n - 1) - 3]
		table.insert(self.verts, a)
		table.insert(self.verts, b)
		table.insert(self.verts, c)
		table.insert(self.verts, a)
		table.insert(self.verts, c)
		table.insert(self.verts, d)
	elseif n == 1 then
		local q1,q2 = self.path[1][1],self.path[1][2]
		local a = {q1.x, q1.y, q1.z, 1}
		local b = {p1.x, p1.y, p1.z, 1}
		local c = {p2.x, p2.y, p2.z, 1}
		local d = {q2.x, q2.y, q2.z, 1}
		table.insert(self.verts, a)
		table.insert(self.verts, b)
		table.insert(self.verts, c)
		table.insert(self.verts, a)
		table.insert(self.verts, c)
		table.insert(self.verts, d)
	end
end

Speedline.remove_slice = function(self)
	table.remove(self.path, 1)
	for i=1,6 do
		table.remove(self.verts, 1)
	end
end

--- Updates the speedline.
-- @param self Speedline.
-- @param secs Seconds since the last update.
Speedline.update = function(self, secs)
	-- Limit the refresh rate.
	--
	-- To avoid rebuilding the speed line unnecessarily often, we enforce
	-- the maximum refresh rate of 40 FPS. That gives moderate smoothness
	-- without consuming all the CPU time.
	self.timer = self.timer + secs
	if self.timer < 1/40 then return end
	local tick = self.timer
	self.timer = 0
	-- Update the delay timer.
	--
	-- This implements the startup delay by exiting immediately as long
	-- as the startup delay phase is active. Once the startup delay has
	-- elapsed, this will be skipped.
	if self.delay > 0 then
		self.delay = self.delay - tick
		if self.delay < 0 then
			self.delay = 0
		else
			return
		end
	end
	-- Realize the render object.
	--
	-- The position of the render object is set here to avoid potential
	-- problems with arrows and other parent objects that may have just
	-- been shown.
	--
	-- The position must be set before updating the path since it is
	-- used for calculating vertex positions. Not setting the path would
	-- lead to the object being positioned at origin, which is undesirable.
	if not self.render:get_visible() then
		if not self.object then
			self:detach()
			return
		end
		self.position = self.object:get_position():copy()
		self.render:set_position(self.position)
		self.render:set_visible(true)
	end
	-- Update the path.
	--
	-- If the parent is visible and the extrusion phase has not ended yet,
	-- create a new slice. Otherwise, remove slices.
	if self.object and self.object:get_visible() and self.duration > 0 then
		self:add_slice()
	else
		self:remove_slice()
	end
	if self.duration > 0 then
		self.duration = self.duration - tick
		if self.duration < 0 then self.duration = 0 end
	end
	-- Remove expired vertices.
	--
	-- If the path is too long, remove the oldest slice from it. If no
	-- slices are left, detach us from the parent.
	local l = self.length
	local n = #self.path
	if n == l then
		self:remove_slice()
		n = n - 1
	elseif n == 0 then
		self:detach()
		return
	end
	-- Update transparency.
	--
	-- This recalculates the alpha component of the color of each vertex.
	-- Oldest vertices have the lowest alpha and the newest the highest.
	local alpha = function(i) return (l-n+i)/l end
	local a1
	local a2 = alpha(0)
	for i = 1,n-1 do
		a1 = a2
		a2 = alpha(i)
		self.verts[6 * i - 5][4] = a2
		self.verts[6 * i - 4][4] = a1
		self.verts[6 * i - 3][4] = a1
		self.verts[6 * i - 2][4] = a2
		self.verts[6 * i - 1][4] = a1
		self.verts[6 * i][4] = a2
	end
	-- Rebuild the model.
	--
	-- This removes the old contents of the models and adds the vertices
	-- created above. The old render model is replaced with the new one
	-- once it has finished building.
	self.model:remove_vertices()
	self.model:add_triangles{material = 1, vertices = self.verts}
	self.model:changed()
	if not self.render:get_loaded() then return end
	local render_model = RenderModel(self.model)
	if self.render.model then
		self.render:replace_model(self.render.model, render_model)
	else
		self.render:add_model(render_model)
	end
	self.render.model_prev = self.render.model
	self.render.model = render_model
end
