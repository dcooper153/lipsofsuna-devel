Quest{
	name = "Sanctuary",
	status = "inactive"}

Obstaclespec{
	name = "sanctuary device",
	model = "torch1",
	categories = {"special"},
	dialog = "sanctuary",
	marker = "sanctuary"}

Pattern{
	name = "sanctuary",
	obstacles = {
		{8,2,8,"sanctuary device"}},
	tiles = {
		{0,0,0,"granite1",14,0,14},-- Base
		{5,0,15,"granite1",3,0,0}, -- Doorstep
		{0,1,0,"granite1",14,5,14}, -- Walls
		{0,7,1,"granite1",14,0,12}, -- Ceiling
		{1,1,1,nil,12,4,12}, -- Inside
		{6,1,14,nil,1,2,1}, -- Door
		{3,2,0,nil,0,1,0}, -- Window
		{11,2,0,nil,0,1,0}, -- Window
		{3,2,14,nil,0,1,0}, -- Window
		{11,2,14,nil,0,1,0}, -- Window
		{0,2,3,nil,0,1,0}, -- Window
		{0,2,7,nil,0,1,0}, -- Window
		{0,2,11,nil,0,1,0}, -- Window
		{14,2,3,nil,0,1,0}, -- Window
		{14,2,7,nil,0,1,0}, -- Window
		{14,2,11,nil,0,1,0}}} -- Window
