Simulation = Class()
Simulation.class_name = "Simulation"

--- Creates an object by a spec.
-- @param self Simulation.
-- @param spec Object spec.
-- @param args Table of argument, or nil.
-- @return Object.
Simulation.create_object_by_spec = function(self, spec, args)
	-- Prepare the arguments.
	local a = {spec = spec, type = spec.type}
	if args then
		a.id = args.id
		a.model = args.model
	end
	-- Create the object.
	local o
	if spec.type == "actor" then
		o = Actor(a)
	elseif spec.type == "item" then
		o = Item(a)
	elseif spec.type == "spell" then
		o = Spell(a)
	elseif spec.type == "obstacle" then
		o = Obstacle(a)
	else
		return
	end
	-- Apply presets.
	if spec.preset then
		local preset = Actorpresetspec:find{name = spec.preset}
		if preset then
			local copy_table = function(t)
				if not t then return end
				local u = {}
				for k,v in pairs(t) do u[k] = v end
				return u
			end
			local copy_color = function(t)
				local u = copy_table(t)
				if not u then return end
				return Color:float_to_ubyte(Color:hsv_to_rgb(u))
			end
			o.body_style = copy_table(preset.body)
			o.eye_color = copy_color(preset.eye_color)
			o.eye_style = preset.eye_style
			o.face_style = copy_table(preset.face)
			o.hair_color = copy_color(preset.hair_color)
			o.hair_style = preset.hair_style
			o.head_style = preset.head_style
			o.body_scale = preset.height
			o.skin_color = copy_color(preset.skin_color)
			o.skin_style = preset.skin_style
		end
	end
	-- Return the object.
	return o
end

--- Updates the simulation.
-- @param self Simulation.
-- @param secs Seconds since the last update.
Simulation.update = function(self, secs)
	-- Update objects.
	--
	-- When a local server is running, it already takes care of updating
	-- the objects, and the update functions also do the client side work.
	-- Hence, this needs not to be done in single player or when hosting
	-- an embedded server.
	if not Server.initialized then
		for k,v in pairs(Object.objects) do
			v:update(secs)
		end
	end
end
