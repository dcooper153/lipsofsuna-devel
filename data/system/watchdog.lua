--- Watchdog timer.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.watchdog
-- @alias Watchdog

local Class = require("system/class")

if not Los.program_load_extension("watchdog") then
	error("loading extension `watchdog' failed")
end

------------------------------------------------------------------------------

--- Watchdog timer.
-- @type Watchdog
local Watchdog = Class("Watchdog")

--- Starts or refreshes the watchdog timer.
-- @param self Watchdog class.
-- @param secs Timer elapsing timer.
Watchdog.start = function(self, secs)
	return Los.watchdog_start(secs)
end

--- Stops the watchdog timer.
-- @param self Watchdog class.
Watchdog.stop = function(self)
	return Los.watchdog_stop()
end

return Watchdog
