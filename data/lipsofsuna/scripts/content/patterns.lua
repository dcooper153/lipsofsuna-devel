Pattern{
	name = "corridor1",
	categories = {"corridor"},
	tiles = {
		{-1,-2,-1,nil,3,3,1},
		{-2,-1,-2,nil,5,5,3},
		{-1,2,-1,nil,3,3,1}}}

Pattern{
	name = "corridor2",
	categories = {"corridor"},
	tiles = {
		{-1,-2,-1,nil,3,3,1},
		{-2,-1,-2,nil,5,5,3},
		{-1,2,-1,nil,3,3,1},
		{-3,0,0,nil,1,1,2},
		{3,0,-1,nil,1,2,2}}}

Pattern{
	name = "corridor3",
	categories = {"corridor"},
	tiles = {
		{-1,-2,-1,nil,3,3,1},
		{-2,-1,-2,nil,5,5,3},
		{-1,2,-1,nil,3,3,1},
		{0,3,0,nil,1,1,2},
		{1,-4,0,nil,1,2,2}}}

Pattern{
	name = "dungeon1",
	obstacles = {
		{2,1,2,"mushroom"},
		{6,1,3,"mushroom"},
		{3,1,5,"mushroom"}},
	tiles = {
		{0,0,0,"granite1",7,0,7},
		{0,1,0,"granite1"},
		{0,2,0,"granite1"},
		{5,1,2,"granite1"},
		{2,1,6,"granite1"}}}

Pattern{
	name = "nature1",
	obstacles = {
		{2,1,2,"tree"},
		{6,1,5,"tree"},
		{3,1,4,"mushroom"}},
	tiles = {
		{0,0,0,"grass1",7,0,7}}}

Pattern{
	name = "house1",
	items = {
		{3,1,1,"air tank"}},
	tiles = {
		{0,0,0,"granite1",6,0,6},-- Base
		{3,0,7,"granite1"}, -- Doorstep
		{0,1,0,"wood1",6,3,6}, -- Walls
		{0,5,1,"wood1",6,0,4}, -- Ceiling
		{1,1,1,nil,4,2,4}, -- Inside
		{3,1,6,nil,0,1,0}, -- Door
		{2,2,0,nil}, -- Window
		{4,2,0,nil}}} -- Window

Pattern{
	name = "spawnpoint1",
	items = {
		{2,2,2,"workbench"}},
	tiles = {
		{0,0,0,"granite1",7,0,7},
		{1,1,1,"granite1",5,0,5}}}
