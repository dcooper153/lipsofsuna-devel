require "system/class"
require "system/math"

if not Los.program_load_extension("graphics") then
	error("loading extension `graphics' failed")
end

------------------------------------------------------------------------------

Client = Class()
Client.class_name = "Client"

--- Clears the screen.
-- @param clss Client class.
Client.clear_buffer = function(clss)
	Los.client_clear_buffer()
end

--- Launches a server.
-- @param clss Client class.
-- @param args Arguments.<ul>
--   <li>1,args: String to pass to the server.</li></ul>
-- @return True on success.
Client.host = function(clss, args)
	return Los.client_host(args)
end

--- Takes a screenshot.
-- @param clss Client class.
-- @return Screenshot filename.
Client.screenshot = function(clss)
	return Los.client_screenshot()
end

--- Sets the current video mode.
-- @param clss Client class.
-- @param ... Arguments.<ul>
--   <li>1,width: Window width in pixels.</li>
--   <li>2,height: Window height in pixels.</li>
--   <li>3,fullscreen: True for fullscreen.</li></ul>
-- @return True on success.
Client.set_video_mode = function(clss, ...)
	return Los.client_set_video_mode(...)
end

--- Copies the rendered scene to the screen.
-- @param clss Client class.
Client.swap_buffers = function(clss)
	Los.client_swap_buffers()
end

--- The current cursor position.
-- @name Client.cursor_pos
-- @class table

--- Short term average frames per second.
-- @name Client.tick
-- @class table

--- Movement mode flag.
-- @name Client.moving
-- @class table

--- Main window title.
-- @name Client.title
-- @class table

--- Current video mode (read-only).
-- @name Client.video_mode
-- @class table

--- List of supported fullscreen modes (read-only).
-- @name Client.video_modes
-- @class table

Client.class_getters = {
	cursor_pos = function(s) return Class.new(Vector, {handle = Los.client_get_cursor_pos()}) end,
	fps = function(s) return Los.client_get_fps() end,
	moving = function(s) return Los.client_get_moving() end,
	video_mode = function(s) return Los.client_get_video_mode() end,
	video_modes = function(s) return Los.client_get_video_modes() end}

Client.class_setters = {
	moving = function(s, v) Los.client_set_moving(v) end,
	title = function(s, v) Los.client_set_title(v) end}
