--- Creates a customized character model for the object.<br/>
-- This function is used by both the network events and the character creation
-- screen to create customized character objects.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>equipment: List of equipment.</li>
--   <li>gender: Gender string.</li>
--   <li>hair_color: Hair color string.</li>
--   <li>hair_style: Hair style string.</li>
--   <li>race: Race string.</li></ul>
Object.create_character_model = function(self, args)
	-- Get the species.
	local name = args.race .. (args.gender == "male" and "m" or "")
	local spec = Species:find{name = name}
	if not spec then return end
	-- Get the base meshes.
	local meshes = {skeleton = spec.model}
	if spec.models then
		for k,v in pairs(spec.models) do
			meshes[k] = v
		end
	end
	-- Add hair model.
	if args.hair_style and args.hair_style ~= "" then
		meshes.hair = args.hair_style
	end
	-- Add equipment models.
	if args.equipment then
		for _,name in pairs(args.equipment) do
			spec = Itemspec:find{name = name}
			if spec and spec.equipment_models then
				for k,v in pairs(spec.equipment_models) do
					meshes[k] = v
				end
			end
		end
	end
	-- Create skeleton.
	local m = Model:load{file = meshes.skeleton}
	m = m:copy()
	m.vertex_policy = "dynamic"
	-- Add other meshes.
	for k,v in pairs(meshes) do
		if k ~= "skeleton" then
			m:merge(Model:load{file = v})
		end
	end
	-- TODO: Colorize hair.
	-- Recalculate bounding box.
	m:calculate_bounds()
	self.model = m
end

Object.update_model = function(self)
	-- Instantiate the model for creatures.
	if self.type == "creature" and self.model and self.model.name then
		self.model = self.model:copy()
	end
	-- Character customizations.
	local spec = self.race and Species:find{name = self.race}
	if spec and spec.models then
		self:create_character_model{
			equipment = self.equipment,
			gender = self.gender,
			hair_color = self.hair_color,
			hair_style = self.hair_style,
			race = self.race}
	end
	-- Particle hacks.
	if self.model_name == "torch1" then
		Thread(function()
			coroutine.yield()
			local fx = Object{model = "torchfx1", position = self.position, realized = true}
			while self.realized do
				local p = self.position
				local n = self:find_node{name = "#flame"}
				if n then p = p + self.rotation * n end
				fx.position = p
				coroutine.yield()
			end
			fx.realized = false
		end)
	end
end
