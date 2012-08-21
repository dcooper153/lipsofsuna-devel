local Program = require("system/core")
local Quaternion = require("system/math/quaternion")
local Vector = require("system/math/vector")

if not Los.program_load_extension("graphics") then
	error("loading extension `graphics' failed")
end

------------------------------------------------------------------------------

--- Takes a screenshot.
-- @param clss Program class.
-- @return Screenshot filename.
Program.capture_screen = function(clss)
	return Los.program_screenshot()
end

--- Layouts a string.
-- @param clss Program class.
-- @param font Font name.
-- @param text Text to layout.
-- @param width Width limit.
-- @return Number list containing (x,y,width) for each glyph.
Program.layout_text = function(clss, font, text, width)
	return Los.program_layout_text(font, text, width)
end

--- Loads the graphics resources.
-- @param clss Program class.
Program.load_graphics = function(clss)
	return Los.program_load_graphics()
end

--- Measures a string.
-- @param clss Program class.
-- @param font Font name.
-- @param text Text to measure.
-- @param width Width limit, or nil.
-- @return Width, height.
Program.measure_text = function(clss, font, text, width)
	return Los.program_measure_text(font, text, width)
end

--- Renders the scene.
-- @param clss Program class.
Program.render_scene = function(clss)
	return Los.program_render()
end

--- Updates the scene.
-- @param clss Program class.
-- @param secs Seconds since the last update.
Program.update_scene = function(clss, secs)
	return Los.program_render_update(secs)
end

--- Gets the far plane distance of the camera.
-- @param self Program class.
-- @return Far plane distance.
Program.get_camera_far = function(self)
	return self.__camera_far or 50
end

--- Sets the far plane distance of the camera.
-- @param self Program class.
-- @param v Far plane distance.
Program.set_camera_far = function(self, v)
	self.__camera_far = v
	Los.render_set_camera_far(v)
end

--- Gets the near plane distance of the camera.
-- @param self Program class.
-- @return Near plane distance.
Program.get_camera_near = function(self)
	return self.__camera_near or 0.1
end

--- Sets the near plane distance of the camera.
-- @param self Program class.
-- @param v Near plane distance.
Program.set_camera_near = function(self, v)
	self.__camera_near = v
	Los.render_set_camera_near(v)
end

--- Gets the position of the camera.
-- @param self Program class.
-- @return Vector.
Program.get_camera_position = function(self)
	return self.__camera_position or Vector()
end

--- Sets the position of the camera.
-- @param self Program class.
-- @param v Vector.
Program.set_camera_position = function(self, v)
	local rot = self:get_camera_rotation()
	self.__camera_position = v
	Los.render_set_camera_transform(v.handle, rot.handle)
end

--- Gets the rotation of the camera.
-- @param self Program class.
-- @return Quaternion.
Program.get_camera_rotation = function(self)
	return self.__camera_rotation or Quaternion()
end

--- Sets the rotation of the camera.
-- @param self Program class.
-- @param v Quaternion.
Program.set_camera_rotation = function(self, v)
	local pos = self:get_camera_position()
	self.__camera_rotation = v
	Los.render_set_camera_transform(pos.handle, v.handle)
end

--- Gets the current video mode.
-- @param self Program class.
-- @return Video mode table.
Program.get_video_mode = function(self)
	return Los.program_get_video_mode()
end

--- Sets the current video mode.
-- @param self Program class.
-- @param width Window width in pixels.
-- @param height Window height in pixels.
-- @param fullscreen True for fullscreen.
-- @return True on success.
Program.set_video_mode = function(self, width, height, fullscreen)
	return Los.program_set_video_mode(width, height, fullscreen)
end

--- Gets the list of supported fullscreen video modes.
-- @param self Program class.
-- @return List of video mode tables.
Program.get_video_modes = function(self)
	return Los.program_get_video_modes()
end

--- Gets the title of the main window.
-- @param self Program class.
-- @return String.
Program.get_window_title = function(self)
	return self.__window_title or ""
end

--- Sets the title of the main window.
-- @param self Program class.
-- @param v String.
Program.set_window_title = function(self, v)
	self.__window_title = v
	Los.program_set_title(v)
end
