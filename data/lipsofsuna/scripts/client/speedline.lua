Speedline = Class(Object)

--- Creates a new speed line effect for the object.
-- @param clss Speedline class.
-- @param object Object.
-- @return Speedline effect.
Speedline.new = function(clss, object)
	self = Object.new(clss)
	self.object = object
	self.path = {}
	self.verts = {}
	self.timer = 0
	self.model = Model()
	self.model:add_material{cull = false, shader = "speedline"}
	return self
end

--- Updates the speedline.
-- @param self Speedline.
-- @param secs Seconds since the last update.
Speedline.update = function(self, secs)
	-- Limit the refresh rate.
	self.timer = self.timer + secs
	if self.timer < 0.03 then return end
	self.timer = 0
	-- Calculate the path length.
	local l = 10
	local h = 0.07
	local n = #self.path
	local rm = function()
		table.remove(self.path, 1)
		for i=1,6 do
			table.remove(self.verts, 1)
		end
	end
	-- Update the path.
	if self.object.realized then
		if not self.realized then
			self.position = self.object.position
			self.realized = true
		end
		local p = self.object.position - self.position
		table.insert(self.path, p)
		if n > 1 then
			local a = self.verts[6 * (n - 1) - 4]
			local b = {p.x, p.y - h, p.z, 1}
			local c = {p.x, p.y + h, p.z, 1}
			local d = self.verts[6 * (n - 1) - 3]
			table.insert(self.verts, a)
			table.insert(self.verts, b)
			table.insert(self.verts, c)
			table.insert(self.verts, a)
			table.insert(self.verts, c)
			table.insert(self.verts, d)
		elseif n == 1 then
			local q = self.path[1]
			local a = {q.x, q.y - h, q.z, 1}
			local b = {p.x, p.y - h, p.z, 1}
			local c = {p.x, p.y + h, p.z, 1}
			local d = {q.x, q.y + h, q.z, 1}
			table.insert(self.verts, a)
			table.insert(self.verts, b)
			table.insert(self.verts, c)
			table.insert(self.verts, a)
			table.insert(self.verts, c)
			table.insert(self.verts, d)
		end
		if n == l then
			rm()
		else
			n = n + 1
		end
	else
		rm()
		n = n - 1
	end
	if n == 0 then
		self.realized = false
		self.object.speedline = nil
	end
	-- Update transparency.
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
	self.model:remove_vertices()
	self.model:add_triangles{material = 1, vertices = self.verts}
	self.model:changed()
end
