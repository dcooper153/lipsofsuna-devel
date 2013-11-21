-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

local Client = require("core/client/client")
local UnlockManager = require("core/unlock/unlock-manager")

Client.reset_hooks:register(5, function()
	Client.data.unlocks = UnlockManager()
end)
