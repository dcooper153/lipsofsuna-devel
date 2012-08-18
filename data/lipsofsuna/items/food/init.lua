Program:add_path(Mod.path)

Itemspec{
	name = "apple",
	categories = {["food"] = true, ["material"] = true},
	mass = 1,
	model = "apple1",
	stacking = true,
	usages = {["eat"] = true}}

CraftingRecipeSpec{
	name = "bread",
	effect = "craftfood1",
	materials = {["wheat"] = 2},
	modes = {["alchemy"] = true}}

Itemspec{
	name = "bread",
	categories = {["food"] = true},
	mass = 1,
	model = "bread2",
	stacking = true,
	usages = {["eat"] = true}}

CraftingRecipeSpec{
	name = "cooked meat",
	effect = "craftfood1",
	materials = {["raw meat"] = 1},
	modes = {["alchemy"] = true}}

Itemspec{
	name = "cooked meat",
	categories = {["food"] = true},
	mass = 1,
	model = "cookedmeat1",
	stacking = true,
	usages = {["eat"] = true}}

Itemspec{
	name = "egg",
	categories = {["food"] = true},
	mass = 1,
	mass_inventory = 0.1,
	model = "egg1",
	stacking = true,
	usages = {["eat"] = true}}

CraftingRecipeSpec{
	name = "long bread",
	effect = "craftfood1",
	materials = {["wheat"] = 2},
	modes = {["alchemy"] = true}}

Itemspec{
	name = "long bread",
	categories = {["food"] = true},
	mass = 1,
	model = "bread1",
	stacking = true,
	usages = {["eat"] = true}}

Itemspec{
	name = "berry",
	categories = {["food"] = true, ["material"] = true},
	mass = 1,
	model = "fruit1",
	stacking = true,
	usages = {["eat"] = true}}

CraftingRecipeSpec{
	name = "cheese",
	effect = "craftfood1",
	materials = {["milky powder"] = 2},--FIXME
	modes = {["alchemy"] = true}}

Itemspec{
	name = "cheese",
	categories = {["food"] = true, ["material"] = true},
	mass = 1,
	model = "cheese1",
	stacking = true,
	usages = {["eat"] = true}}

Itemspec{
	name = "fruit",
	categories = {["food"] = true, ["material"] = true},
	mass = 1,
	model = "fruit1",
	stacking = true,
	usages = {["eat"] = true}}

Itemspec{
	name = "orange",
	categories = {["food"] = true, ["material"] = true},
	mass = 1,
	model = "orange1",
	stacking = true,
	usages = {["eat"] = true}}

Itemspec{
	name = "raw meat",
	categories = {["food"] = true},
	mass = 1,
	model = "rawmeat1",
	stacking = true,
	usages = {["eat"] = true}}

CraftingRecipeSpec{
	name = "small cooked meat",
	effect = "craftfood1",
	materials = {["small raw meat"] = 1},
	modes = {["alchemy"] = true}}

Itemspec{
	name = "small cooked meat",
	categories = {["food"] = true},
	mass = 1,
	model = "cookedmeat2",
	stacking = true,
	usages = {["eat"] = true}}

Itemspec{
	name = "small raw meat",
	categories = {["food"] = true},
	mass = 1,
	model = "rawmeat2",
	stacking = true,
	usages = {["eat"] = true}}

CraftingRecipeSpec{
	name = "rice bowl",
	effect = "craftfood1",
	materials = {["wheat"] = 2},--FIXME
	modes = {["alchemy"] = true}}

Itemspec{
	name = "rice bowl",
	categories = {["food"] = true},
	mass = 1,
	model = "ricebowl1",
	stacking = true,
	usages = {["eat"] = true}}
