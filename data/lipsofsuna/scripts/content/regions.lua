local r

-- The town of Lips.
Regionspec{
	name = "root",
	size = {32,32,32},
	style = "lips"}

-- Dungeon room.
Regionspec{
	name = "dungeon-room",
	size = {7,6,7},
	style = "cave"}

-- Dungeon corridor along North-South axis.
Regionspec{
	name = "dungeon-corridor-ns",
	size = {6,7,10},
	style = "cave"}

-- Dungeon corridor along East-West axis.
Regionspec{
	name = "dungeon-corridor-ew",
	size = {10,7,6},
	style = "cave"}

-- Dungeon slope rising toward East.
Regionspec{
	name = "dungeon-slope-e",
	size = {3,8,3},
	style = "cave"}

-- Dungeon slope rising toward West.
Regionspec{
	name = "dungeon-slope-w",
	size = {3,8,3},
	style = "cave"}

-- Dungeon slope rising toward North.
Regionspec{
	name = "dungeon-slope-n",
	size = {3,8,3},
	style = "cave"}

-- Dungeon slope rising toward South.
Regionspec{
	name = "dungeon-slope-s",
	size = {3,8,3},
	style = "cave"}

Regionspec{
	name = "rootsofworld-grove",
	size = {8,8,8},
	style = "rootgrove"}

-- Connectivity rules for the town of Lips.
r = Regionspec:find{name = "root"}
r:link{name = "dungeon-corridor-ns", dirs = {"n", "s"}}
r:link{name = "dungeon-corridor-ew", dirs = {"e", "w"}}
-- Connectivity rules for the dungeon theme.
r = Regionspec:find{name = "dungeon-room"}
r:link{name = "dungeon-corridor-ns", back = true, dirs = {"n", "s"}}
r:link{name = "dungeon-corridor-ew", back = true, dirs = {"e", "w"}}
r:link{name = "dungeon-slope-n", dirs = {"n"}, y = -4}
r:link{name = "dungeon-slope-n", dirs = {"s"}, y = -2}
r:link{name = "dungeon-slope-s", dirs = {"n"}, y = -2}
r:link{name = "dungeon-slope-s", dirs = {"s"}, y = -4}
r:link{name = "dungeon-slope-e", dirs = {"e"}, y = -4}
r:link{name = "dungeon-slope-e", dirs = {"w"}, y = -2}
r:link{name = "dungeon-slope-w", dirs = {"e"}, y = -2}
r:link{name = "dungeon-slope-w", dirs = {"w"}, y = -4}
r = Regionspec:find{name = "dungeon-corridor-ew"}
r:link{name = "dungeon-corridor-ew", back = true, dirs = {"e", "w"}}
r:link{name = "dungeon-corridor-ns", back = true, dirs = {"n", "s"}, align = 0.4}
r:link{name = "dungeon-corridor-ns", back = true, dirs = {"n", "s"}, align = 0.6}
r:link{name = "dungeon-slope-n", back = true, dirs = {"n"}, y = -1, align = 0.5}
r:link{name = "dungeon-slope-n", back = true, dirs = {"s"}, y = -4, align = 0.5}
r:link{name = "dungeon-slope-s", back = true, dirs = {"n"}, y = -4, align = 0.5}
r:link{name = "dungeon-slope-s", back = true, dirs = {"s"}, y = -1, align = 0.5}
r:link{name = "dungeon-slope-e", back = true, dirs = {"e"}, y = -3}
r:link{name = "dungeon-slope-w", back = true, dirs = {"e"}, y = -3}
r:link{name = "dungeon-slope-e", back = true, dirs = {"w"}, y = 3}
r:link{name = "dungeon-slope-w", back = true, dirs = {"w"}, y = 3}
r:link{name = "rootsofworld-grove", back = true, dirs = {"e", "w"}}
r = Regionspec:find{name = "dungeon-corridor-ns"}
r:link{name = "dungeon-slope-s", back = true, dirs = {"s"}, y = -3}
r:link{name = "dungeon-slope-n", back = true, dirs = {"s"}, y = -3}
r:link{name = "dungeon-slope-s", back = true, dirs = {"n"}, y = 3}
r:link{name = "dungeon-slope-n", back = true, dirs = {"n"}, y = 3}
r:link{name = "rootsofworld-grove", back = true, dirs = {"n", "s"}}
