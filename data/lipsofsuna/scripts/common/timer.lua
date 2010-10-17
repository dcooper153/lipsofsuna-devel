Timer = Class()
Timer.timers = {}
setmetatable(Timer.timers, {__mode = "v"})

Timer.setter = function(self, key, value)
	if key == "enabled" then
		if value then
			self:enable()
		else
			self:disable()
		end
	else
		Class.setter(self, key, value)
	end
end

--- Creates a new timer and enables it.<br/>
-- The timer will not be subject to garbage collection when enabled.
-- @param clss Timer class.
-- @param args Arguments.<ul>
--   <li>accurate: True to trigger multiple times in one frame to compensate for low frame rate.</li>
--   <li>delay: Delay in seconds or nil.</li>
--   <li>enabled: False to disable by default.</li>
--   <li>func: Callback function.</li>
--   <li>owner: Owner object or nil.</li></ul>
-- @return New timer.
Timer.new = function(clss, args)
	local self = Class.new(clss, args)
	if args.enabled == nil then
		self:enable()
	end
	return self
end

--- Disables the timer.<br/>
-- The timer will be subject to normal garbage collection when disabled.
-- @param self Timer.
Timer.disable = function(self)
	Timer.timers[self] = nil
end

--- Enables the timer.<br/>
-- The timer will not be subject to garbage collection when enabled.
-- @param self Timer.
Timer.enable = function(self)
	Timer.timers[self] = self.owner or true
	self.updated = Program.time
end

-- Register event handler.
Eventhandler{type = "tick", func = function(self, args)
	local t = Program.time
	for k,v in pairs(Timer.timers) do
		local delay = (k.delay or 0)
		local target = k.updated + delay
		if target <= t then
			if k.accurate and delay > 0 then
				while target <= t do
					k:func(delay)
					k.updated = target
					target = target + delay
				end
			else
				k:func(t - target)
				k.updated = t
			end
		end
	end
end}

