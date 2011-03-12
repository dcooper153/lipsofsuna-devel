Material{
	mining_materials = {["adamantium"] = 1},
	name = "adamantium1",
	shader = "adamantium",
	shininess = 100,
	texture = {"adamantium1", "stone-001"},
	texture_scale = 0.33,
	type = "rounded"}

Material{
	mining_materials = {["aquanite stone"] = 1},
	name = "aquanite1",
	shader = "terrain",
	shininess = 80,
	texture = {"aquanite1", "crimson1n", "stone-001"},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	mining_materials = {["basalt stone"] = 1},
	name = "basalt1",
	shader = "terrain",
	texture = {"basalt1", "basalt1n", "stone-001"},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	effect_collapse = "collapse1",
	name = "brick1",
	shader = "terrain",
	texture = {"bricks3", "bricks3n", "stone-001"},
	texture_scale = 0.11,
	type = "cube"}

Material{
	effect_collapse = "collapse1",
	name = "brittlerock1",
	shader = "terrain",
	texture = {"granite1", "granite1n", "stone-001"}, -- FIXME
	texture_scale = 0.11,
	type = "rounded"}

Material{
	effect_collapse = "collapse1",
	name = "cobbles1",
	shader = "terrain",
	texture = {"cobbles1", "cobbles1n", "stone-001"},
	texture_scale = 0.13,
	type = "rounded"}

Material{
	effect_collapse = "collapse1",
	mining_materials = {["granite stone"] = 1},
	name = "concrete1",
	shader = "terrain",
	texture = {"granite1", "granite1n", "stone-001"},
	texture_scale = 0.33,
	type = "cube"}

Material{
	mining_materials = {["crimson stone"] = 1},
	name = "crimson1",
	shader = "terrain",
	shininess = 80,
	texture = {"crimson1", "crimson1n", "stone-001"},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	effect_collapse = "collapse2",
	mining_materials = {["ferrostone"] = 1},
	name = "ferrostone1",
	shader = "terrain",
	shininess = 60,
	texture = {"ferrostone1", "ferrostone1n", "stone-001"},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	effect_collapse = "collapse1",
	mining_materials = {["granite stone"] = 1},
	name = "granite1",
	shader = "terrain",
	texture = {"granite1", "granite1n", "stone-001"},
	texture_scale = 0.11,
	type = "rounded fractal"}

Material{
	effect_collapse = "collapse2",
	mining_transform = "soil1",
	name = "grass1",
	shader = "terrain",
	texture = {"grass-000", "grass-000-nml", "stone-001"},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	name = "ice1",
	shader = "terrain",
	shininess = 100,
	specular = {1,1,1,1},
	texture = {"ice1", "ice1n", "stone-001"},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	mining_materials = {["iron ingot"] = 1},
	name = "iron1",
	shader = "terrain",
	texture = {"metal-000", "ice1n", "metal-000"}} -- FIXME

Material{
	name = "magma1",
	cullface = false,
	magma = true,
	shader = "lava",
	texture = {"noise1", "noise1", "stone-001"},
	type = "liquid"}

Material{
	mining_materials = {["iron ingot"] = 1},
	name = "pipe1",
	shader = "terrain",
	texture = {"wood1", "wood1n", "stone-001"}, -- FIXME
	type = "rounded"}

Material{
	effect_collapse = "collapse2",
	mining_materials = {["sand chunk"] = 1},
	name = "sand1",
	shader = "terrain",
	texture = {"sand1", "sand1n", "stone-001"},
	texture_scale = 0.22,
	type = "rounded"}

Material{
	effect_collapse = "collapse2",
	mining_materials = {["soil chunk"] = 1},
	name = "soil1",
	shader = "terrain",
	texture = {"soil1", "soil1n", "stone-001"},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	name = "water1",
	cullface = false,
	shader = "water",
	texture = {"noise1", "noise1", "stone-001"},
	type = "liquid"}

Material{
	effect_collapse = "collapse2",
	mining_materials = {["log"] = 1},
	name = "wood1",
	shader = "terrain",
	texture = {"wood1", "wood1n", "stone-001"},
	texture_scale = 0.11}
