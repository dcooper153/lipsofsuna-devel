Regionspec = Class()
Regionspec.dict_name = {}

--- Finds a region specification.
-- @param clss Regionspec class.
-- @param args Arguments.<ul>
--   <li>name: Region name.</li></ul>
-- @return Regionspec or nil.
Regionspec.find = function(clss, args)
	return clss.dict_name[args.name]
end

--- Creates a new region specification.
-- @param clss Regionspec class.
-- @param args Arguments.<ul>
--   <li>links: Array of connectivity rules.</li>
--   <li>name: Unique region name.</li>
--   <li>size: Region size in tiles.</li>
--   <li>style: Interior style.</li></ul>
-- @return New regionspec.
Regionspec.new = function(clss, args)
	local self = Class.new(clss, args)
	self.links = self.links or {}
	self.size = self.size or {4,4,4}
	self.style = self.style or "default"
	clss.dict_name[args.name] = self
	return self
end

--- Links regions to this one.
-- @param self Regionspec.
-- @param args Arguments.<ul>
--   <li>align: Position along the placed side.</li>
--   <li>back: True to enable back linking.</li>
--   <li>dirs: Array of linking directions.</li>
--   <li>name: Regionspec name.</li>
--   <li>size: Size of the linked region.</li>
--   <li>y: Y offset in tiles.</li></ul>
Regionspec.link = function(self, args)
	local r = Regionspec:find{name = args.name}
	local a = args.align or 0.5
	local s = (r and r.size) or args.size or {7,7,7}
	local y = args.y or 0
	local funcs = {
		e = function()
			table.insert(self.links, {{args.name,
				-r.size[1], y, math.floor((self.size[3] - r.size[3]) * a)}})
			if args.back then
				table.insert(r.links, {{self.name,
					r.size[1], -y, math.floor((r.size[3] - self.size[3]) * (1 - a))}})
			end
		end,
		n = function()
			table.insert(self.links, {{args.name,
				math.floor((self.size[1] - r.size[1]) * a), y, self.size[3]}})
			if args.back then
				table.insert(r.links, {{self.name,
					math.floor((r.size[1] - self.size[1]) * (1 - a)), -y, -self.size[3]}})
			end
		end,
		s = function()
			table.insert(self.links, {{args.name,
				math.floor((self.size[1] - r.size[1]) * a), y, -r.size[3]}})
			if args.back then
				table.insert(r.links, {{self.name,
					math.floor((r.size[1] - self.size[1]) * (1 - a)), -y, r.size[3]}})
			end
		end,
		w = function()
			table.insert(self.links, {{args.name,
				self.size[1], y, math.floor((self.size[3] - r.size[3]) * a)}})
			if args.back then
				table.insert(r.links, {{self.name,
					-self.size[1], -y, math.floor((r.size[3] - self.size[3]) * (1 - a))}})
			end
		end}
	for _,d in pairs(args.dirs) do
		local func = funcs[d]
		if func then func() end
	end
end
