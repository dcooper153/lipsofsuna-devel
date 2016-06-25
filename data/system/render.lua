--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.render
-- @alias Render

local Class = require("system/class")
local Vector = require("system/math/vector")

if not Los.program_load_extension("graphics") then
	error("loading extension `graphics' failed")
end

if not Los.program_load_extension("render") then
	error("loading extension `render' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type Render
local Render = Class("Render")

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
	return Vector:new_from_handle(r)
end

--- Gets the anisotrophic filtering setting.
-- @param self Render class.
-- @return Number.
Render.get_anisotrophy = function(self)
	return Los.render_get_anisotrophy()
end

--- Gets the anisotrophic filtering setting.
-- @param self Render class.
-- @param v Number.
Render.set_anisotrophy = function(self, v)
	Los.render_set_anisotrophy(v)
end

Render.get_material_scheme = function(s)
	return rawget(s, "__material_scheme") or "Default"
end

Render.set_material_scheme = function(s, v)
	rawset(s, "__material_scheme", v)
	Los.render_set_material_scheme(v)
end

Render.get_scene_ambient = function(s)
	return rawget(s, "__scene_ambient") or {0.5,0.5,0.5,1.0}
end

Render.set_scene_ambient = function(s, v)
	rawset(s, "__scene_ambient", v)
	Los.render_set_scene_ambient(v)
end

Render.get_skybox = function(s)
	return rawget(s, "__skybox")
end

Render.set_skybox = function(s, v)
	rawset(s, "__skybox", v)
	Los.render_set_skybox(v)
end

Render.get_stats = function(self)
	return Los.render_get_stats()
end

Render.update_texture = function(self, name, image)
	if image then
		Los.render_update_texture(name, image.handle)
	end
end

return Render


