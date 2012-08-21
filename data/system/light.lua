local Class = require("system/class")
local Quaternion = require("system/math/quaternion")
local Vector = require("system/math/vector")

if not Los.program_load_extension("graphics") then
	error("loading extension `render' failed")
end

if not Los.program_load_extension("render") then
	error("loading extension `render' failed")
end

------------------------------------------------------------------------------

local Light = Class("Light")

--- Creates a new light source.
-- @param clss Light class.
-- @return New light source.
Light.new = function(clss)
	local self = Class.new(clss)
	self.handle = Los.light_new()
	self.__ambient = {0,0,0,1}
	self.__diffuse = {1,1,1,1}
	self.__enabled = false
	self.__equation = {1,1,1}
	self.__position = Vector()
	self.__priority = 0
	self.__rotation = Quaternion()
	self.__shadow_casting = false
	self.__shadow_far = 0.1
	self.__shadow_near = 50
	self.__specular = {1,1,1,1}
	self.__spot_cutoff = math.pi
	self.__spot_exponent = 0
	return self
end

--- Gets the ambient color of the light source.
-- @param self Light.
-- @return Color table.
Light.get_ambient = function(self)
	return self.__ambient
end

--- Sets the ambient color of the light source.
-- @param self Light.
-- @param v Color table.
Light.set_ambient = function(self, v)
	self.__ambient = v
	Los.light_set_ambient(self.handle, v)
end

--- Gets the diffuse color of the light source.
-- @param self Light.
-- @return Color table.
Light.get_diffuse = function(self)
	return self.__diffuse
end

--- Sets the diffuse color of the light source.
-- @param self Light.
-- @param v Color table.
Light.set_diffuse = function(self, v)
	self.__diffuse = v
	Los.light_set_diffuse(self.handle, v)
end

--- Gets the directionality state of the light.
-- @param self Light.
-- @return Boolean.
Light.get_directional = function(self)
	return self.__directional
end

--- Sets the directionality state of the light.
-- @param self Light.
-- @param v Boolean.
Light.set_directional = function(self, v)
	self.__directional = v
	Los.light_set_directional(self.handle, v)
end

--- Gets the enable state of the light.
-- @param self Light.
-- @return Boolean.
Light.get_enabled = function(self)
	return self.__enabled
end

--- Sets the enable state of the light.
-- @param self Light.
-- @param v Boolean.
Light.set_enabled = function(self, v)
	self.__enabled = v
	Los.light_set_enabled(self.handle, v)
end

--- Gets the attenuation equation of the light.
-- @param self Light.
-- @return List of three numbers.
Light.get_equation = function(self)
	return self.__equation
end

--- Sets the attenuation equation of the light.
-- @param self Light.
-- @param v List of three numbers.
Light.set_equation = function(self, v)
	self.__equation = v
	Los.light_set_equation(self.handle, v)
end

--- Gets the position of the light.
-- @param self Light.
-- @return Vector.
Light.get_position = function(self)
	return self.__position
end

--- Sets the position of the light.
-- @param self Light.
-- @param v Vector.
Light.set_position = function(self, v)
	self.__position = v
	Los.light_set_position(self.handle, v.handle)
end

--- Gets the priority of the light.<br/>
--
-- Higher means more important.
--
-- @param self Light.
-- @return Number.
Light.get_priority = function(self)
	return self.__priority
end

--- Gets the priority of the light.<br/>
--
-- Higher means more important.
--
-- @param self Light.
-- @param v Number.
Light.set_priority = function(self, v)
	self.__priority = v
	Los.light_set_priority(self.handle, v)
end

--- Gets the rotation of the light.
-- @param self Light.
-- @return Quaternion.
Light.get_rotation = function(self)
	return self.__rotation
end

--- Sets the rotation of the object.
-- @param self Object.
-- @param v Quaternion.
Light.set_rotation = function(self, v)
	self.__rotation = v
	Los.light_set_rotation(self.handle, v.handle)
end

--- Gets the shadow casting enable state of the light.
-- @param self Light.
-- @return Boolean.
Light.get_shadow_casting = function(self)
	return self.__shadow_casting
end

--- Sets the shadow casting enable state of the light.
-- @param self Light.
-- @param v Boolean.
Light.set_shadow_casting = function(self, v)
	self.__shadow_casting = v
	Los.light_set_shadow_casting(self.handle, v)
end

--- Gets the far plane distance of the shadow projection.
-- @param self Light.
-- @return Number.
Light.get_shadow_far = function(self)
	return self.__shadow_far
end

--- Sets the far plane distance of the shadow projection.
-- @param self Light.
-- @param v.
Light.set_shadow_far = function(self, v)
	self.__shadow_far = v
	Los.light_set_shadow_far(self.handle, v)
end

--- Gets the near plane distance of the shadow projection.
-- @param self Light.
-- @return Number.
Light.get_shadow_near = function(self)
	return self.__shadow_near
end

--- Sets the near plane distance of the shadow projection.
-- @param self Light.
-- @param v Number.
Light.set_shadow_near = function(self, v)
	self.__shadow_far = v
	Los.light_set_shadow_near(self.handle, v)
end

--- Gets the specular color of the light.
-- @param self Light.
-- @return Color table.
Light.get_specular = function(self)
	return self.__specular
end

--- Sets the specular color of the light.
-- @param self Light.
-- @param v Color table.
Light.set_specular = function(self, v)
	self.__specular = v
	Los.light_set_specular(self.handle, v)
end

--- Gets the spot cutoff angle of the light, in radians.
-- @param self Light.
-- @return Number.
Light.get_spot_cutoff = function(self)
	return self.__spot_cutoff
end

--- Sets the spot cutoff angle of the light, in radians.
-- @param self Light.
-- @param v Number.
Light.set_spot_cutoff = function(self, v)
	self.__spot_cutoff = v
	Los.light_set_spot_cutoff(self.handle, v)
end

--- Gets the spot exponent of the light.
-- @param self Light.
-- @return Number.
Light.get_spot_exponent = function(self)
	return self.__spot_exponent
end

--- Sets the spot exponent of the light.
-- @param self Light.
-- @param v Number.
Light.set_spot_exponent = function(self, v)
	self.__spot_exponent = v
	Los.light_set_spot_exponent(self.handle, v)
end

return Light
