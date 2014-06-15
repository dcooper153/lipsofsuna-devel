--- Chat module initialization.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.chat.init
-- @alias ChatInit

local ChatManager = require("core/chat/chat-manager")

Main.chat = ChatManager()
require("core/chat/commands")
