--- Manages the windows of the user interface.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.window_manager
-- @alias WindowManager

local Class = require("system/class")
local Client = require("core/client/client")
local Graphics = require("system/graphics")
local Program = require("system/core")
local Time = require("system/time")
local Vector = require("system/math/vector")
local Widget = require("system/widget")

--- Manages the windows of the user interface.
-- @type WindowManager
local WindowManager = Class("WindowManager")

--- Creates a new window manager.
-- @param clss WindowManager class.
-- @return WindowManager.
WindowManager.new = function(clss)
	local self = Class.new(clss)
	self.__windows = {}
	self.__base = Widget()
	self.__base:set_floating(true)
	self.__base:set_fullscreen(true)
	local mode = Graphics:get_video_mode()
	self.__width = mode[1]
	self.__height = mode[2]
	return self
end

--- Adds a window.
-- @param self WindowManager.
-- @param window Window.
-- @param hack FIXME.
WindowManager.add_window = function(self, window, hack)
	self.__windows[window] = true
	if not hack then
		self.__base:add_child(window)
	end
	window:screen_resized(self.__width, self.__height)
end

--- Handles an input event.
-- @param self WindowManager.
-- @param args Event arguments.
-- @return True if the caller should handle the event. False otherwise.
WindowManager.handle_event = function(self, args)
	for k,v in pairs(self.__windows) do
		if not k:handle_event(args) then
			return false
		end
	end
	return true
end

--- Updates the user interface.
-- @param self WindowManager.
-- @param secs Seconds since the last update.
WindowManager.update = function(self, secs)
	-- Update the window size.
	local mode = Graphics:get_video_mode()
	local resized = (mode[1] ~= self.__width or mode[2] ~= self.__height)
	if resized then
		self.__width = mode[1]
		self.__height = mode[2]
		self.__base:set_request(mode[1], mode[2])
		self.__base:set_offset(Vector(0, 0))
		for k,v in pairs(self.__windows) do
			k:screen_resized(mode[1], mode[2])
		end
	end
	-- Emit key repeat events.
	local t = Time:get_secs()
	for k,v in pairs(Client.input.pressed) do
		if t - v.time > 0.05 then
			v.type = "keyrepeat"
			v.mods = Client.input.mods
			v.time = t
			self:handle_event(v)
		end
	end
	-- Update the user interface state.
	for k,v in pairs(self.__windows) do
		k:update(secs)
	end
	-- Update video mode options.
	if resized then
		local v = Graphics:get_video_mode()
		Client.options.window_width = v[1]
		Client.options.window_height = v[2]
		Client.options.fullscreen = v[3]
		Client.options.vsync = v[4]
		Client.options:save()
	end
end

return WindowManager
