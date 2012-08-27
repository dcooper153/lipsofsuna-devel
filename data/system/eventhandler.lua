--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.eventhandler
-- @alias Eventhandler

local Class = require("system/class")
local Packet = require("system/packet")
local Program = require("system/core")

--- TODO:doc
-- @type Eventhandler
local Eventhandler = Class("Eventhandler")
Eventhandler.handlers = setmetatable({}, {__mode = "v"})

--- Creates a new event handler and enables it.<br/>
-- The event handler will not be subject to garbage collection when enabled.
-- @param clss Event handler class.
-- @param args Arguments.<ul>
--   <li>func: Callback function. (required)</li>
--   <li>owner: Owner object.</li>
--   <li>type: Event type. (required)</li></ul>
-- @return New event handler.
Eventhandler.new = function(clss, args)
	local self = Class.new(clss)
	for k,v in pairs(args) do self[k] = v end
	self:enable()
	return self
end

--- Disables the event handler.<br/>
-- The event handler will be subject to normal garbage collection when disabled.
-- @param self Event handler.
Eventhandler.disable = function(self)
	Eventhandler.handlers[self] = nil
end

--- Enables the event handler.<br/>
-- The event handler will not be subject to garbage collection when enabled.
-- @param self Event handler.
Eventhandler.enable = function(self)
	Eventhandler.handlers[self] = self.owner or true
end

--- Handles an event.
-- @param clss Event handler class.
-- @param args Event arguments.
Eventhandler.event = function(clss, args)
	-- Translate handles.
	if args.type == "packet" then
		args.packet = Packet:new_from_handle(args.packet)
		args.packet:read()
	end
	for k,v in pairs(args) do
		if type(v) == "userdata" then
			-- TODO: Update this if some events can create other kinds of userdata.
			local d = __userdata_lookup[v]
			if d then
				args[k] = d
			else
				args[k] = Vector:new_from_handle(v)
			end
		end
	end
	-- Invoke event handlers.
	for k,v in pairs(clss.handlers) do
		if k.type == args.type then
			Program:safe_call(function() k:func(args) end)
		end
	end
end

--- Reads events from the program and dispatches them.
-- @param clss Event handler class.
Eventhandler.update = function(clss)
	local event = Program:pop_event()
	while event do
		clss:event(event)
		event = Program:pop_event()
	end
end

return Eventhandler


