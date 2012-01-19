Material{
	effect_build = "builtrock1",
	mining_materials = {["adamantium"] = 1},
	name = "adamantium1",
	material = "adamantium",
	shininess = 100,
	texture = {"adamantium1", "stone-001"},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	effect_build = "builtrock1",
	mining_materials = {["aquanite stone"] = 1},
	name = "aquanite1",
	material = "terrain1",
	shininess = 80,
	texture = {"aquanite1", "crimson1n", "stone-001"},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	effect_build = "builtrock1",
	mining_materials = {["basalt stone"] = 1},
	name = "basalt1",
	material = "terrain1",
	texture = {"basalt1", "basalt1n", "stone-001"},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	effect_build = "builtrock1",
	effect_collapse = "collapse1",
	name = "brick1",
	material = "terrain1",
	specular = {0.5,0.5,0.5,0.5},
	texture = {"bricks3", "bricks3n", "stone-001"},
	texture_scale = 0.11,
	type = "cube"}

Material{
	effect_build = "builtrock1",
	effect_collapse = "collapse1",
	name = "brick1slope",
	material = "terrain1",
	specular = {0.5,0.5,0.5,0.5},
	texture = {"bricks3", "bricks3n", "bricks3"},
	texture_scale = 0.11,
	type = "sloped"}

Material{
	effect_build = "builtrock1",
	effect_collapse = "collapse1",
	name = "brittlerock1",
	material = "terrain1",
	specular = {0.5,0.5,0.5,0.5},
	texture = {"granite1", "granite1n", "stone-001"}, -- FIXME
	texture_scale = 0.11,
	type = "rounded"}

Material{
	effect_build = "builtrock1",
	effect_collapse = "collapse1",
	name = "cobbles1",
	material = "terrain1",
	specular = {0.5,0.5,0.5,0.5},
	texture = {"cobbles1", "cobbles1n", "stone-001"},
	texture_scale = 0.11,
	type = "sloped"}

Material{
	effect_build = "builtrock1",
	effect_collapse = "collapse1",
	mining_materials = {["granite stone"] = 1},
	name = "concrete1",
	material = "terrain1",
	specular = {0.5,0.5,0.5,0.5},
	texture = {"granite1", "concrete1n", "stone-001"},
	texture_scale = 0.11,
	type = "cube"}

Material{
	effect_build = "builtrock1",
	effect_collapse = "collapse1",
	mining_materials = {["granite stone"] = 1},
	name = "concrete2",
	material = "terrain1",
	specular = {0.5,0.5,0.5,0.5},
	texture = {"granite1", "concrete1n", "stone-001"},
	texture_scale = 0.11,
	type = "sloped"}

Material{
	effect_build = "builtrock1",
	mining_materials = {["crimson stone"] = 1},
	name = "crimson1",
	material = "terrain1",
	shininess = 80,
	texture = {"crimson1", "crimson1n", "stone-001"},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	effect_build = "builtrock1",
	effect_collapse = "collapse2",
	mining_materials = {["ferrostone"] = 1},
	name = "ferrostone1",
	material = "terrain1",
	shininess = 60,
	texture = {"ferrostone1", "ferrostone1n", "stone-001"},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	effect_build = "builtrock1",
	effect_collapse = "collapse1",
	mining_materials = {["granite stone"] = 1},
	name = "granite1",
	material = "terrain1",
	specular = {0.5,0.5,0.5,0.5},
	texture = {"granite1", "granite1n", "stone-001"},
	texture_scale = 0.11,
	type = "rounded fractal"}

Material{
	effect_build = "builtrock1",
	effect_collapse = "collapse2",
	mining_transform = "soil1",
	name = "grass1",
	material = "terrain1",
	specular = {0.5,0.5,0.5,0.5},
	texture = {"grass-000", "grass-000-nml", "stone-001"},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	effect_build = "builtrock1",
	effect_collapse = "collapse2",
	mining_transform = "soil1",
	name = "grass2",
	material = "terrain1",
	specular = {0.5,0.5,0.5,0.5},
	texture = {"grass-000", "grass-000-nml", "grass-000"},
	texture_scale = 0.11,
	type = "sloped"}

Material{
	name = "ice1",
	material = "terrain1",
	shininess = 100,
	texture = {"ice1", "ice1n", "stone-001"},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	mining_materials = {["iron ingot"] = 1},
	name = "iron1",
	material = "terrain1",
	texture = {"metal-000", "ice1n", "metal-000"}} -- FIXME

Material{
	name = "magma1",
	cullface = false,
	magma = true,
	material = "lava",
	texture = {"noise1", "noise1", "stone-001"},
	type = "liquid"}

Material{
	mining_materials = {["iron ingot"] = 1},
	name = "pipe1",
	material = "terrain1",
	texture = {"wood1", "wood1n", "stone-001"}, -- FIXME
	texture_scale = 0.11,
	type = "rounded"}

Material{
	effect_build = "builtrock1",
	effect_collapse = "collapse2",
	mining_materials = {["sand chunk"] = 1},
	name = "sand1",
	material = "terrain1",
	specular = {0.5,0.5,0.5,0.5},
	texture = {"sand1", "sand1n", "stone-001"},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	effect_build = "builtrock1",
	effect_collapse = "collapse2",
	mining_materials = {["soil chunk"] = 1},
	name = "soil1",
	material = "terrain1",
	specular = {0.5,0.5,0.5,0.5},
	texture = {"soil1", "soil1n", "stone-001"},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	name = "water1",
	cullface = false,
	material = "water",
	texture = {"noise1", "noise1", "stone-001"},
	texture_scale = 0.11,
	type = "liquid"}

Material{
	name = "water2",
	cullface = false,
	shader = "swamp",
	texture = {"noise1", "noise1", "stone-001"},
	texture_scale = 0.11,
	type = "liquid"}

Material{
	effect_build = "builtwood1",
	effect_collapse = "collapse2",
	mining_materials = {["log"] = 1},
	name = "wood1",
	material = "terrain1",
	specular = {0.5,0.5,0.5,0.5},
	texture = {"wood1", "wood1n", "stone-001"},
	texture_scale = 0.11}

Material{
	effect_build = "builtwood1",
	effect_collapse = "collapse2",
	mining_materials = {["log"] = 1},
	name = "wood1sloped",
	material = "terrain1",
	specular = {0.5,0.5,0.5,0.5},
	texture = {"wood1", "wood1n", "wood1"},
	texture_scale = 0.11,
	type = "sloped"}

Material{
	effect_build = "builtwood1",
	effect_collapse = "collapse2",
	mining_materials = {["log"] = 1},
	name = "wood1smooth",
	material = "terrain1",
	specular = {0.5,0.5,0.5,0.5},
	texture = {"wood1", "wood1n", "wood1"},
	texture_scale = 0.11,
	type = "rounded"}
