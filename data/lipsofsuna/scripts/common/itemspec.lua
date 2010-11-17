require "common/spec"

Itemspec = Class(Spec)
Itemspec.type = "item"
Itemspec.dict_id = {}
Itemspec.dict_cat = {}
Itemspec.dict_name = {}

--- Creates a new item specification.
-- @param clss Itemspec class.
-- @param args Arguments.<ul>
--   <li>categories: List of categories to which the item belongs.</li>
--   <li>crafting_count: Number of items crafted per use.</li>
--   <li>crafting_materials: Dictionary of crafting materials.</li>
--   <li>crafting_skills: Dictionary of required crafting skills.</li>
--   <li>damage: Base damage.</li>
--   <li>damage_bonus_dexterity: How much one point of dexterity adds to damage.</li>
--   <li>damage_bonus_health: How much one point of health adds to damage.</li>
--   <li>damage_bonus_intelligence: How much one point of intelligence adds to damage.</li>
--   <li>damage_bonus_percention: How much one point of perception adds to damage.</li>
--   <li>damage_bonus_strength: How much one point of strength adds to damage.</li>
--   <li>damage_bonus_willpower: How much one point of willpower adds to damage.</li>
--   <li>destroy_actions: List of actions to perform when the item is destroyed.</li>
--   <li>destroy_timer: Time in seconds after which to destruct when thrown.</li>
--   <li>equipment_models: Dictionary of equipment models or nil.</li>
--   <li>equipment_slot: Equipment slot into which the item can be placed.</li>
--   <li>health: Number of hit points the item has.</li>
--   <li>icon: Icon name.</li>
--   <li>mass: Mass in kilograms.</li>
--   <li>model: Model to use for the item. (required)</li>
--   <li>name: Name of the item type. (required)</li>
--   <li>stacking: True to allow the item to stack in the inventory.</li></ul>
-- @return New item specification.
Itemspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.crafting_enabled = (self.crafting_count or self.crafting_materials or self.crafting_skills) and true
	self.crafting_count = self.crafting_count or 1
	self.crafting_materials = self.crafting_materials or {}
	self.crafting_skills = self.crafting_skills or {}
	self.destroy_actions = self.destroy_actions or {}
	self.mass = self.mass or 10
	-- Models.
	if args.equipment_models then
		self.equipment_models = {}
		for k,v in pairs(args.equipment_models) do
			self.equipment_models[k] = v
		end
	end
	return self
end

--- Returns a random item spec.
-- @param clss Itemspec class.
-- @param args Arguments.<ul>
--   <li>category: Category name.</li></ul>
-- @return Itemspec or nil.
Itemspec.random = function(clss, args)
	if args and args.category then
		local cat = clss.dict_cat[args.category]
		if not cat then return end
		return cat[math.random(#cat)]
	end
	return clss.dict_id[math.random(#clss.dict_id)]
end
