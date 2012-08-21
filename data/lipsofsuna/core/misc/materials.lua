local Material = require("system/material")

Material{
	name = "adamantium1",
	material = "adamantium1",
	effect_build = "builtrock1",
	mining_materials = {["adamantium"] = 1},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	name = "aquanite1",
	material = "aquanite1",
	effect_build = "builtrock1",
	mining_materials = {["aquanite stone"] = 1},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	name = "basalt1",
	material = "basalt1",
	effect_build = "builtrock1",
	mining_materials = {["basalt stone"] = 1},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	name = "brick1",
	material = "brick1",
	effect_build = "builtrock1",
	effect_collapse = "collapse1",
	texture_scale = 0.11,
	type = "cube"}

Material{
	name = "brick1slope",
	material = "brick1",
	effect_build = "builtrock1",
	effect_collapse = "collapse1",
	texture_scale = 0.11,
	type = "sloped"}

Material{
	name = "brittlerock1",
	material = "granite1", -- FIXME
	effect_build = "builtrock1",
	effect_collapse = "collapse1",
	texture_scale = 0.11,
	type = "rounded"}

Material{
	name = "cobbles1",
	material = "cobbles1",
	effect_build = "builtrock1",
	effect_collapse = "collapse1",
	texture_scale = 0.11,
	type = "sloped"}

Material{
	name = "concrete1",
	material = "concrete1",
	effect_build = "builtrock1",
	effect_collapse = "collapse1",
	mining_materials = {["granite stone"] = 1},
	texture_scale = 0.11,
	type = "cube"}

Material{
	name = "concrete2",
	material = "concrete1",
	effect_build = "builtrock1",
	effect_collapse = "collapse1",
	mining_materials = {["granite stone"] = 1},
	texture_scale = 0.11,
	type = "sloped"}

Material{
	name = "crimson1",
	material = "crimson1",
	effect_build = "builtrock1",
	mining_materials = {["crimson stone"] = 1},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	name = "ferrostone1",
	material = "ferrostone1",
	effect_build = "builtrock1",
	effect_collapse = "collapse2",
	mining_materials = {["ferrostone"] = 1},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	name = "granite1",
	material = "granite1",
	effect_build = "builtrock1",
	effect_collapse = "collapse1",
	mining_materials = {["granite stone"] = 1},
	texture_scale = 0.11,
	type = "rounded fractal"}

Material{
	name = "grass1",
	material = "grass1",
	effect_build = "builtrock1",
	effect_collapse = "collapse2",
	mining_transform = "soil1",
	texture_scale = 0.11,
	type = "rounded"}

Material{
	name = "grass2",
	material = "grass2",
	effect_build = "builtrock1",
	effect_collapse = "collapse2",
	mining_transform = "soil1",
	texture_scale = 0.11,
	type = "sloped"}

Material{
	name = "ice1",
	material = "ice1",
	texture_scale = 0.11,
	type = "rounded"}

Material{
	name = "iron1",
	material = "iron1",
	mining_materials = {["iron ingot"] = 1}}

Material{
	name = "magma1",
	material = "magma1",
	magma = true,
	type = "liquid"}

Material{
	name = "pipe1",
	material = "iron1", -- FIXME
	mining_materials = {["iron ingot"] = 1},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	name = "sand1",
	material = "sand1",
	effect_build = "builtrock1",
	effect_collapse = "collapse2",
	mining_materials = {["sand chunk"] = 1},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	name = "soil1",
	material = "soil1",
	effect_build = "builtrock1",
	effect_collapse = "collapse2",
	mining_materials = {["soil chunk"] = 1},
	texture_scale = 0.11,
	type = "rounded"}

Material{
	name = "water1",
	material = "water1",
	texture_scale = 0.11,
	type = "liquid"}

Material{
	name = "water2",
	material = "water1", -- FIXME
	texture_scale = 0.11,
	type = "liquid"}

Material{
	name = "wood1",
	material = "wood1",
	effect_build = "builtwood1",
	effect_collapse = "collapse2",
	mining_materials = {["log"] = 1},
	texture_scale = 0.11}

Material{
	name = "wood1sloped",
	material = "wood1",
	effect_build = "builtwood1",
	effect_collapse = "collapse2",
	mining_materials = {["log"] = 1},
	texture_scale = 0.11,
	type = "sloped"}

Material{
	name = "wood1smooth",
	material = "wood1",
	effect_build = "builtwood1",
	effect_collapse = "collapse2",
	mining_materials = {["log"] = 1},
	texture_scale = 0.11,
	type = "rounded"}
