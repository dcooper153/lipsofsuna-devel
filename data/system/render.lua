require "system/class"
require "system/math"

if not Los.program_load_extension("render") then
	error("loading extension `render' failed")
end

------------------------------------------------------------------------------

Light = Class()
Light.class_name = "Light"

--- Creates a new light source.
-- @param clss Light class.
-- @param args Arguments.
-- @return New light source.
Light.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.light_new()
	if self.handle then
		__userdata_lookup[self.handle] = self
	end
	rawset(self, "__ambient", {0,0,0,1})
	rawset(self, "__diffuse", {1,1,1,1})
	rawset(self, "__enabled", false)
	rawset(self, "__equation", {1,1,1})
	rawset(self, "__position", Vector())
	rawset(self, "__priority", 0)
	rawset(self, "__rotation", Quaternion())
	rawset(self, "__shadow_casting", false)
	rawset(self, "__shadow_far", 0.1)
	rawset(self, "__shadow_near", 50)
	rawset(self, "__specular", {1,1,1,1})
	rawset(self, "__spot_cutoff", math.pi)
	rawset(self, "__spot_exponent", 0)
	if args then
		for k,v in pairs(args) do self[k] = v end
	end
	return self
end

--- The ambient color of the light source.
-- @name Light.ambient
-- @class table

--- The diffuse color of the light source.
-- @name Light.diffuse
-- @class table

--- Sets the light directional.
-- @name Light.directional
-- @class table

--- Enables or disables the light.
-- @name Light.enabled
-- @class table

--- The attenuation equation of the light source.
-- @name Light.equation
-- @class table

--- Gets or sets the position of the light.
-- @name Light.position
-- @class table

--- The priority of the light, higher means more important.
-- @name Light.priority
-- @class table

--- Gets or sets the rotation of the light.
-- @name Light.rotation
-- @class table

--- Enables or disables shadow casting.
-- @name Light.shadow_casting
-- @class table

--- Far place distance of the shadow projection.
-- @name Light.shadow_far
-- @class table

--- Near place distance of the shadow projection.
-- @name Light.shadow_near
-- @class table

--- The specular color of the light source.
-- @name Light.specular
-- @class table

--- Spot cutoff angle of the light, in radians.
-- @name Light.spot_cutoff
-- @class table

--- Spot exponent of the light.
-- @name Light.spot_cutoff
-- @class table

Light:add_getters{
	ambient = function(s) return rawget(s, "__ambient") end,
	diffuse = function(s) return rawget(s, "__diffuse") end,
	directional = function(s) return rawget(s, "__directional") end,
	enabled = function(s) return rawget(s, "__enabled") end,
	equation = function(s) return rawget(s, "__equation") end,
	position = function(s) return rawget(s, "__position") end,
	priority = function(s) return rawget(s, "__priority") end,
	rotation = function(s) return rawget(s, "__rotation") end,
	shadow_casting = function(s) return rawget(s, "__shadow_casting") end,
	shadow_far = function(s) return rawget(s, "__shadow_far") end,
	shadow_near = function(s) return rawget(s, "__shadow_near") end,
	specular = function(s) return rawget(s, "__specular") end,
	spot_cutoff = function(s) return rawget(s, "__spot_cutoff") end,
	spot_exponent = function(s) return rawget(s, "__spot_exponent") end}

Light:add_setters{
	ambient = function(s, v)
		rawset(s, "__ambient", v)
		Los.light_set_ambient(s.handle, v)
	end,
	diffuse = function(s, v)
		rawset(s, "__diffuse", v)
		Los.light_set_diffuse(s.handle, v)
	end,
	directional = function(s, v)
		rawset(s, "__directional", v)
		Los.light_set_directional(s.handle, v)
	end,
	enabled = function(s, v)
		rawset(s, "__enabled", v)
		Los.light_set_enabled(s.handle, v)
	end,
	equation = function(s, v)
		rawset(s, "__equation", v)
		Los.light_set_equation(s.handle, v)
	end,
	position = function(s, v)
		rawset(s, "__position", v)
		Los.light_set_position(s.handle, v.handle)
	end,
	priority = function(s, v)
		rawset(s, "__priority", v)
		Los.light_set_priority(s.handle, v)
	end,
	rotation = function(s, v)
		rawset(s, "__rotation", v)
		Los.light_set_rotation(s.handle, v.handle)
	end,
	shadow_casting = function(s, v)
		rawset(s, "__shadow_casting", v)
		Los.light_set_shadow_casting(s.handle, v)
	end,
	shadow_far = function(s, v)
		rawset(s, "__shadow_far", v)
		Los.light_set_shadow_far(s.handle, v)
	end,
	shadow_near = function(s, v)
		rawset(s, "__shadow_far", v)
		Los.light_set_shadow_near(s.handle, v)
	end,
	specular = function(s, v)
		rawset(s, "__specular", v)
		Los.light_set_specular(s.handle, v)
	end,
	spot_cutoff = function(s, v)
		rawset(s, "__spot_cutoff", v)
		Los.light_set_spot_cutoff(s.handle, v)
	end,
	spot_exponent = function(s, v)
		rawset(s, "__spot_exponent", v)
		Los.light_set_spot_exponent(s.handle, v)
	end}

------------------------------------------------------------------------------

Model:add_getters{
	render_loaded = function(self)
		return Los.model_get_render_loaded(self.handle)
	end}

------------------------------------------------------------------------------

Render = Class()
Render.class_name = "Render"

--- Enables a compositor script.
-- @param self Render class.
-- @param name Compositor script name.
Render.add_compositor = function(self, name)
	Los.render_add_compositor(name)
end

--- Disables a compositor script.
-- @param self Render class.
-- @param name Compositor script name.
Render.remove_compositor = function(self, name)
	Los.render_remove_compositor(name)
end

--- Projects a point to the screen space.
-- @param self Render class.
-- @param point Vector.
-- @return Vector.
Render.project = function(self, point)
	local r = Los.render_project(point.handle)
	return Class.new(Vector, {handle = r})
end

--- Anisotrophic filtering setting.
-- @name Render.anisotrophy
-- @class table

Render.class_getters = {
	anisotrophy = function(s) return Los.render_get_anisotrophy() end,
	material_scheme = function(s) return rawget(s, "__material_scheme") or "Default" end,
	scene_ambient = function(s) return rawget(s, "__scene_ambient") or {0.5,0.5,0.5,1.0} end,
	skybox = function(s) return rawget(s, "__skybox") end,
	stats = function(self) return Los.render_get_stats() end}

Render.class_setters = {
	anisotrophy = function(s, v) Los.render_set_anisotrophy(v) end,
	material_scheme = function(s, v)
		rawset(s, "__material_scheme", v)
		Los.render_set_material_scheme(v)
	end,
	scene_ambient = function(s, v)
		rawset(s, "__scene_ambient", v)
		Los.render_set_scene_ambient(v)
	end,
	skybox = function(s, v)
		rawset(s, "__skybox", v)
		Los.render_set_skybox(v)
	end}
