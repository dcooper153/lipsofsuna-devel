--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module client.quickslots
-- @alias Quickslots

local Class = require("system/class")
local Widget = require("system/widget")
local Quickpage = require("core/quickslots/quickpage")

--- TODO:doc
-- @type Quickslots
local Quickslots = Class("Quickslots")

Quickslots.init = function(clss)
	clss.group = Widget()
	clss.items = Quickpage{type = "items"}
	clss.feats = Quickpage{type = "feats"}
	clss.mode = "feats"
end

--- Assigns a feat to a quickslot.
-- @param clss Quickslots class.
-- @param index Quickslot index number.
-- @param feat Feat to assign or nil.
Quickslots.assign_feat = function(clss, index, feat)
	if feat then
		clss.feats:assign_feat(index, feat)
	else
		clss.feats:assign_none(index)
	end
	clss.mode = "feats"
end

--- Clears the assignment of the quickslot.
-- @param clss Quickslots class.
-- @param index Quickslot index number.
Quickslots.assign_none = function(clss, index)
	clss.items:assign_none(index)
end

--- Assigns an item to a quickslot.
-- @param clss Quickslots class.
-- @param index Quickslot index number.
-- @param item Item to assign or nil.
Quickslots.assign_item = function(clss, index, item)
	if item then
		clss.items:assign_item(index, item)
	else
		clss.items:assign_none(index)
	end
end

--- Called when the quickslot is clicked or accessed by a hotkey.
-- @param clss Quickslots class.
-- @param index Quickslot index number.
Quickslots.activate = function(clss, index)
	if clss.mode == "feats" then
		clss.feats:activate(index)
	else
		clss.items:activate(index)
	end
end

--- Resets the quickslots to the original state.
-- @param clss Quickslots class.
Quickslots.reset = function(clss)
	for i = 1,10 do
		clss.items:assign_none(i)
	end
end

--- Saves the quickslots.
-- @param clss Quickslots class.
Quickslots.save = function(clss)
	clss.feats:save()
end

return Quickslots
