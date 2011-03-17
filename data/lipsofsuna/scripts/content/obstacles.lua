Obstaclespec{
	name = "tree",
	model = "tree1",
	categories = {"tree"},
	destroy_items = {
		{"log",Vector(0,0,0)},
		{"log",Vector(0,1,0)},
		{"log",Vector(0,2,0)},
		{"log",Vector(0,3,0)}},
	health = 50}

Obstaclespec{
	name = "rust sprig tree",
	model = "tree2",
	categories = {"tree"},
	destroy_items = {
		{"log",Vector(0,0,0)},
		{"log",Vector(0,1,0)},
		{"log",Vector(0,2,0)},
		{"log",Vector(0,3,0)}},
	health = 50}

Obstaclespec{
	name = "mushroom",
	model = "mushroom-000",
	categories = {"small-plant", "reagent"},
	health = 10,
	harvest_behavior = "destroy",
	harvest_effect = "harvest1",
	harvest_materials = { mushroom = 1 }}
