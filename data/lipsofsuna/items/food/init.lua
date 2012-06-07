Program:add_path(Mod.path)

Itemspec{
	name = "apple",
	model = "apple1",
	mass = 1,
	categories = {["food"] = true, ["material"] = true},
	stacking = true}

Itemspec{
	name = "bread",
	model = "bread2",
	mass = 1,
	categories = {["food"] = true},
	crafting_materials = {["wheat"] = 2},
	stacking = true}

Itemspec{
	name = "cooked meat",
	model = "cookedmeat1",
	mass = 1,
	categories = {["food"] = true},
	crafting_materials = {["raw meat"] = 1},
	stacking = true}

Itemspec{
	name = "egg",
	model = "egg1",
	mass = 1,
	mass_inventory = 0.1,
	categories = {["food"] = true},
	stacking = true}

Itemspec{
	name = "long bread",
	model = "bread1",
	mass = 1,
	categories = {["food"] = true},
	crafting_materials = {["wheat"] = 2},
	stacking = true}

Itemspec{
	name = "berry",
	model = "fruit1",
	mass = 1,
	categories = {["food"] = true, ["material"] = true},
	stacking = true}

Itemspec{
	name = "cheese",
	model = "cheese1",
	mass = 1,
	categories = {["food"] = true, ["material"] = true},
	crafting_materials = {["milky powder"] = 2}, --FIXME
	stacking = true}

Itemspec{
	name = "fruit",
	model = "fruit1",
	mass = 1,
	categories = {["food"] = true, ["material"] = true},
	stacking = true}

Itemspec{
	name = "orange",
	model = "orange1",
	mass = 1,
	categories = {["food"] = true, ["material"] = true},
	stacking = true}

Itemspec{
	name = "raw meat",
	model = "rawmeat1",
	mass = 1,
	categories = {["food"] = true},
	stacking = true}

Itemspec{
	name = "small cooked meat",
	model = "cookedmeat2",
	mass = 1,
	categories = {["food"] = true},
	crafting_materials = {["small raw meat"] = 1},
	stacking = true}

Itemspec{
	name = "small raw meat",
	model = "rawmeat2",
	mass = 1,
	categories = {["food"] = true},
	stacking = true}

Itemspec{
	name = "rice bowl",
	model = "ricebowl1",
	mass = 1,
	categories = {["food"] = true},
	crafting_materials = {["wheat"] = 2}, --FIXME
	stacking = true}
