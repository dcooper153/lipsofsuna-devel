local getters = {
	angular = function(s) return s:get_angular() end,
	collision_group = function(s) return s:get_collision_group() end,
	collision_mask = function(s) return s:get_collision_mask() end,
	contact_events = function(s) return s:get_contact_events() end,
	gravity = function(s) return s:get_gravity() end,
	ground = function(s) return s:get_ground() end,
	mass = function(s) return s:get_mass() end,
	model = function(s) return s:get_model() end,
	model_name = function(s)
		local m = s:get_model()
		return m and m.name or ""
	end,
	movement = function(s) return Object.get_movement(s) end,
	physics = function(s) return s:get_physics() end,
	position = function(s) return s:get_position() end,
	rotation = function(s) return s:get_rotation() end,
	realized = function(s) return s:get_realized() end,
	sector = function(s) return s:get_sector() end,
	shape = function(s) return s:get_shape() end,
	speed = function(s) return s:get_speed() end,
	strafing = function(s) return Object.get_strafing(s) end,
	velocity = function(s) return s:get_velocity() end}

local setters = {
	angular = function(s, v) s:set_angular(v) end,
	collision_group = function(s, v) s:set_collision_group(v) end,
	collision_mask = function(s, v) s:set_collision_mask(v) end,
	contact_events = function(s, v) s:set_contact_events(v) end,
	gravity = function(s, v) s:set_gravity(v) end,
	ground = function(s, v) s:set_ground(v) end,
	mass = function(s, v) s:set_mass(v) end,
	model = function(s, v)
		if type(v) == "string" then
			s:set_model(Model:load{file = v, mesh = s.load_meshes})
		else
			s:set_model(v)
		end
	end,
	model_name = function(s, v)
		s:set_model(Model:load{file = v, mesh = s.load_meshes})
	end,
	movement = function(s, v) Object.set_movement(s, v) end,
	physics = function(s, v) s:set_physics(v) end,
	position = function(s, v) s:set_position(v) end,
	rotation = function(s, v) s:set_rotation(v) end,
	realized = function(s, v) s:set_realized(v) end,
	sector = function(s, v) s:set_sector(v) end,
	shape = function(s, v) s:set_shape(v) end,
	speed = function(s, v) s:set_speed(v) end,
	strafing = function(s, v) Object.set_strafing(s, v) end,
	velocity = function(s, v) s:set_velocity(v) end}

Object.getters = getters
Object.setters = setters

Object.getter = function(self, key)
	local objectgetterfunc = getters[key]
	if objectgetterfunc then return objectgetterfunc(self) end
	return Class.getter(self, key)
end

Object.setter = function(self, key, value)
	local objectsetterfunc = setters[key]
	if objectsetterfunc then return objectsetterfunc(self, value) end
	return Class.setter(self, key, value)
end
