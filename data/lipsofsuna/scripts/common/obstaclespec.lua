Obstaclespec = Class(Spec)
Obstaclespec.type = "obstacle"
Obstaclespec.dict_id = {}
Obstaclespec.dict_cat = {}
Obstaclespec.dict_name = {}

--- Creates a new obstacle specification.
-- @param clss Obstaclespec class.
-- @param args Arguments.<ul>
--   <li>categories: List of categories to which the obstacle belongs.</li>
--   <li>harvest_behavior: Harvest behavior. ("keep"/"destroy")</li>
--   <li>harvest_effect: Effect to play when harvested.</li>
--   <li>harvest_materials: Dictionary of harvestable materials.</li>
--   <li>destroy_actions: List of actions to perform when the obstacle is destroyed.</li>
--   <li>health: Number of hit points the obstacle has.</li>
--   <li>mass: Mass in kilograms.</li>
--   <li>model: Model to use for the obstacle. (required)</li>
--   <li>name: Name of the obstacle type. (required)</li>
--   <li>physics: Physics mode.</li></ul>
-- @return New item specification.
Obstaclespec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.harvest_enabled = args.harvest_materials ~= nil
	self.harvest_behavior = self.harvest_behavior or "keep"
	self.harvest_materials = self.harvest_materials or {}
	self.destroy_actions = self.destroy_actions or {}
	self.mass = self.mass or 10
	self.physics = self.physics or "static"
	return self
end

--- Returns a random obstacle spec.
-- @param clss Obstaclespec class.
-- @param args Arguments.<ul>
--   <li>category: Category name.</li></ul>
-- @return Obstaclespec or nil.
Obstaclespec.random = function(clss, args)
	if args and args.category then
		local cat = clss.dict_cat[args.category]
		if not cat then return end
		return cat[math.random(#cat)]
	end
	return clss.dict_id[math.random(#clss.dict_id)]
end
