-- The town of Lips.
Regionspec{
	name = "root",
	size = {16,16,16},
	style = "lips",
	links = {
		{{"dungeon-corridor-ns",7,0,-10}},
		{{"dungeon-corridor-ns",7,0,16}},
		{{"dungeon-corridor-ew",-10,0,7}},
		{{"dungeon-corridor-ew",16,0,7}}}}

-- Dungeon room.
Regionspec{
	name = "dungeon-room",
	size = {7,6,7},
	style = "cave",
	links = {
		{{"dungeon-corridor-ns",2,0,-10}},
		{{"dungeon-corridor-ns",2,0,7}},
		{{"dungeon-corridor-ew",-10,0,2}},
		{{"dungeon-corridor-ew",7,0,2}}}}

-- Dungeon corridor along North-South axis.
Regionspec{
	name = "dungeon-corridor-ns",
	size = {3,4,10},
	style = "cave",
	links = {
		{{"dungeon-corridor-ns",0,0,-10}},
		{{"dungeon-corridor-ns",0,0,10}},
		{{"dungeon-corridor-ew",-10,0,4}},
		{{"dungeon-corridor-ew",3,0,4}},
		{{"dungeon-slope-n",0,-3,-3}},
		{{"dungeon-slope-s",0,-1,-3}},
		{{"dungeon-slope-n",0,-1,10}},
		{{"dungeon-slope-s",0,-3,10}},
		{{"dungeon-room",-2,0,-7}},
		{{"dungeon-room",-2,0,10}}}}

-- Dungeon corridor along East-West axis.
Regionspec{
	name = "dungeon-corridor-ew",
	size = {10,4,3},
	style = "cave",
	links = {
		{{"dungeon-corridor-ew",-10,0,0}},
		{{"dungeon-corridor-ew",10,0,0}},
		{{"dungeon-corridor-nw",4,0,-10}},
		{{"dungeon-corridor-nw",4,0,3}},
		{{"dungeon-slope-e",-3,-3,0}},
		{{"dungeon-slope-w",-3,-1,0}},
		{{"dungeon-slope-e",10,-1,0}},
		{{"dungeon-slope-w",10,-3,0}},
		{{"dungeon-room",-7,0,-2}},
		{{"dungeon-room",10,0,-2}}}}

-- Dungeon slope rising toward East.
Regionspec{
	name = "dungeon-slope-e",
	size = {3,8,3},
	style = "cave",
	links = {
		{{"dungeon-corridor-ew",-10,1,0}},
		{{"dungeon-corridor-ew",3,4,0}},
		{{"dungeon-corridor-ns",-10,1,0}},
		{{"dungeon-corridor-ns",-10,1,-7}},
		{{"dungeon-corridor-ns",3,4,0}},
		{{"dungeon-corridor-ns",3,4,-7}},
		{{"dungeon-slope-e",-3,-2,0}},
		{{"dungeon-slope-w",-3,0,0}},
		{{"dungeon-slope-e",3,2,0}},
		{{"dungeon-slope-w",3,0,0}}}}

-- Dungeon slope rising toward West.
Regionspec{
	name = "dungeon-slope-w",
	size = {3,8,3},
	style = "cave",
	links = {
		{{"dungeon-corridor-ew",-10,4,0}},
		{{"dungeon-corridor-ew",3,1,0}},
		{{"dungeon-corridor-ns",-3,4,0}},
		{{"dungeon-corridor-ns",-3,4,-7}},
		{{"dungeon-corridor-ns",3,1,0}},
		{{"dungeon-corridor-ns",3,1,-7}},
		{{"dungeon-slope-w",-3,2,0}},
		{{"dungeon-slope-e",-3,0,0}},
		{{"dungeon-slope-w",3,-2,0}},
		{{"dungeon-slope-e",3,0,0}}}}

-- Dungeon slope rising toward North.
Regionspec{
	name = "dungeon-slope-n",
	size = {3,8,3},
	style = "cave",
	links = {
		{{"dungeon-corridor-ns",0,1,-10}},
		{{"dungeon-corridor-ns",0,4,3}},
		{{"dungeon-corridor-ew",0,1,-3}},
		{{"dungeon-corridor-ew",-7,1,-3}},
		{{"dungeon-corridor-ew",0,4,3}},
		{{"dungeon-corridor-ew",-7,4,3}},
		{{"dungeon-slope-n",0,-2,-3}},
		{{"dungeon-slope-s",0,0,-3}},
		{{"dungeon-slope-n",0,2,3}},
		{{"dungeon-slope-s",0,0,3}}}}

-- Dungeon slope rising toward South.
Regionspec{
	name = "dungeon-slope-s",
	size = {3,8,3},
	style = "cave",
	links = {
		{{"dungeon-corridor-ns",0,4,-10}},
		{{"dungeon-corridor-ns",0,1,3}},
		{{"dungeon-corridor-ew",0,4,-3}},
		{{"dungeon-corridor-ew",-7,4,-3}},
		{{"dungeon-corridor-ew",0,1,3}},
		{{"dungeon-corridor-ew",-7,1,3}},
		{{"dungeon-slope-s",0,2,-3}},
		{{"dungeon-slope-n",0,0,-3}},
		{{"dungeon-slope-s",0,-2,3}},
		{{"dungeon-slope-n",0,0,3}}}}
