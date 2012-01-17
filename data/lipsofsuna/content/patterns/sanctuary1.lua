Pattern{
	name = "sanctuary1",
	size = Vector(32,24,32),
	obstacles = {
		{17,2,17,"sanctuary device"}},
	tiles = {
		{9,0,9,"concrete1",14,0,14},-- Base
		{14,0,24,"concrete1",3,0,0}, -- Doorstep
		{9,1,9,"brick1",14,5,14}, -- Walls
		{9,7,10,"concrete1",14,0,12}, -- Ceiling
		{10,1,10,nil,12,4,12}, -- Inside
		{15,1,23,nil,1,2,1}, -- Door
		{12,2,9,nil,0,1,0}, -- Window
		{20,2,9,nil,0,1,0}, -- Window
		{12,2,23,nil,0,1,0}, -- Window
		{20,2,23,nil,0,1,0}, -- Window
		{9,2,12,nil,0,1,0}, -- Window
		{9,2,16,nil,0,1,0}, -- Window
		{9,2,20,nil,0,1,0}, -- Window
		{23,2,12,nil,0,1,0}, -- Window
		{23,2,16,nil,0,1,0}, -- Window
		{23,2,20,nil,0,1,0}}} -- Window
