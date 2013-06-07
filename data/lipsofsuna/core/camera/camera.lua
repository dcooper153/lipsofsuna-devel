local Class = require("system/class")

local Camera = Class("CameraOperator")
Camera.data = {}

--- Initializes the camera operator.
--
-- Context: The game must have been joined and the character created.
--
-- @param self Operator.
Camera.reset = function(self)
	self.data = {}
end

--- Returns true if the camera is in the rotation mode.
--
-- Context: The game must have been joined and the character created.
--
-- @param self Operator.
-- @return True if being rotated.
Camera.get_rotation_mode = function(self)
	return self.data.rotating
end

--- Enables or disables the camera rotation mode.
--
-- Context: The game must have been joined and the character created.
--
-- @param self Operator.
-- @param value True to enable rotation.
Camera.set_rotation_mode = function(self, value)
	self.data.rotating = value
end

return Camera
