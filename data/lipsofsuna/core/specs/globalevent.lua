--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.globalevent
-- @alias Globaleventspec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- TODO:doc
-- @type Globaleventspec
Globaleventspec = Spec:register("Globaleventspec", "global event", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "duration", type = "number", description = "Duration of the event, in seconds."},
	{name = "sector_created", type = "ignore"},
	{name = "sector_loaded", type = "ignore"},
	{name = "started", type = "ignore"},
	{name = "stopped", type = "ignore"},
	{name = "update", type = "ignore"},
})

--- Creates a new global event specification.
-- @param clss Globaleventspec class.
-- @param args Arguments.
-- @return New global event spec.
Globaleventspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Called when a new sector is created or loaded.
-- @param clss Globaleventspec class.
-- @param event Global event instance.
-- @param id Chunk ID.
-- @param loaded True for loaded, false for newly created.
-- @param objects List of objects in the sector.
-- @return True to prevent other events from running the same function.
Globaleventspec.sector_created = function(clss, event, id, loaded, objects)
end

--- Started when the global event is started.
-- @param clss Globaleventspec class.
-- @param event Global event instance.
Globaleventspec.started = function(clss, event)
end

--- Started when the global event is stopped.
-- @param clss Globaleventspec class.
-- @param event Global event instance.
Globaleventspec.stopped = function(clss, event)
end

--- Called periodically to update the event.
-- @param clss Globaleventspec class.
-- @param event Global event instance.
-- @param secs Seconds since the last update.
Globaleventspec.update = function(clss, event, secs)
end


