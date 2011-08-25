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
	__userdata_lookup[self.handle] = self
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

Light.unittest = function()
	-- Position.
	local l = Light{position = Vector(1,2,3)}
	assert(l.position.x == 1)
	assert(l.position.y == 2)
	assert(l.position.z == 3)
	-- Equation.
	l.equation = {3,2,1}
	assert(l.equation[1] == 3)
	assert(l.equation[2] == 2)
	assert(l.equation[3] == 1)
	-- Enabling.
	l.enabled = false
	assert(l.enabled == false)
	l.enabled = true
	assert(l.enabled == true)
end

------------------------------------------------------------------------------

Render = Class()
Render.class_name = "Render"

--- Anisotrophic filtering setting.
-- @name Render.anisotrophy
-- @class table

Render.class_getters = {
	anisotrophy = function(s) return Los.render_get_anisotrophy() end}

Render.class_setters = {
	anisotrophy = function(s, v) Los.render_set_anisotrophy(v) end}

------------------------------------------------------------------------------

Scene = Class()
Scene.class_name = "Scene"

--- Creates a new scene rendering buffer.
-- @param clss Scene class.
-- @param args Arguments.
-- @return Scene.
Scene.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.scene_new()
	__userdata_lookup[self.handle] = self
	if args then
		for k,v in pairs(args) do self[k] = v end
	end
	return self
end

--- Renders the scene.
-- @param self Scene.
-- @param args Arguments.<ul>
--   <li>hdr: True to enable HDR.</li>
--   <li>modelview: Modelview matrix.</li>
--   <li>multisamples: Number of multisamples. Pass arguments:</li>
--   <li>postproc_passes: Array of post-processing passes.<ul>
--     <li>mipmaps: True to generate mipmaps.</li>
--     <li>shader: Post-processing shader name.</li></ul></li>
--   <li>projection: Projection matrix.</li>
--   <li>render_passes: Array of render passes. Pass arguments:<ul>
--     <li>pass: Pass number.</li>
--     <li>sorting: True to enable sorting.</li></ul></li>
--   <li>viewport: Viewport array.</li></ul>
Scene.render = function(self, args)
	Los.scene_render(self.handle, args)
end

------------------------------------------------------------------------------

Shader = Class()
Shader.class_name = "Shader"

--- Creates a new shader.
-- @param clss Shader class.
-- @param args Arguments.<ul>
--   <li>name: Unique shader name.</li>
--   <li>sort: True to allow depth sorting for materials with this shader.</li>
--   <li>transform_feedback: True to enable transform feedback.</li>
--   <li>passN_blend: True to enable blending.</li>
--   <li>passN_blend_src: Source blend function.</li>
--   <li>passN_blend_dst: Destination blend function.</li>
--   <li>passN_depth_test: False to disable depth test.</li>
--   <li>passN_depth_write: False to disable depth writes.</li>
--   <li>passN_depth_func: Depth test function.</li>
--   <li>passN_fragment: Fragment program code.</li>
--   <li>passN_geometry: Geometry program code.</li>
--   <li>passN_vertex: Vertex program code.</li></ul>
-- @return New shader.
Shader.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.shader_new(args)
	__userdata_lookup[self.handle] = self
	return self
end

--- Recompiles the shader from new source code.
-- @param self Shader.
-- @param args Arguments.<ul>
--   <li>sort: True to allow depth sorting for materials with this shader.</li>
--   <li>transform_feedback: True to enable transform feedback.</li>
--   <li>passN_blend: True to enable blending.</li>
--   <li>passN_blend_src: Source blend function.</li>
--   <li>passN_blend_dst: Destination blend function.</li>
--   <li>passN_depth_test: False to disable depth test.</li>
--   <li>passN_depth_write: False to disable depth writes.</li>
--   <li>passN_depth_func: Depth test function.</li>
--   <li>passN_fragment: Fragment program code.</li>
--   <li>passN_geometry: Geometry program code.</li>
--   <li>passN_vertex: Vertex program code.</li></ul>
Shader.compile = function(self, args)
	Los.shader_compile(self.handle, args)
end
