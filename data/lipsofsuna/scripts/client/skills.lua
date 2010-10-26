Skills = Class()
Skills.dict_row = {}
Skills.dict_id = {}

Skills.init = function(clss)
	local dialog = Widget{cols = 2, rows = 2}
	dialog.list = Widgets.List()
	dialog.list.pressed = function(view, row) Skills:show(row) end
	dialog.skill_name = Widgets.Label{font = "medium"}
	dialog.skill_desc = Widgets.Label()
	dialog.skill_desc:set_request{width = 300}
	dialog.group2 = Widget{cols = 1, rows = 3}
	dialog.group2:set_expand{row = 3}
	dialog.group2:set_child{col = 1, row = 1, widget = dialog.skill_name}
	dialog.group2:set_child{col = 1, row = 2, widget = dialog.skill_desc}
	dialog.spacings = {40, 5}
	dialog:set_expand{col = 2, row = 1}
	dialog:set_child{col = 1, row = 2, widget = dialog.list}
	dialog:set_child{col = 2, row = 2, widget = dialog.group2}
	clss.window = dialog
end

--- Adds a skill.
-- @param clss Skills class.
-- @param id Skill id.
-- @param name Skill name.
-- @param desc Skill description.
Skills.add = function(clss, id, name, desc)
	local index = #clss.dict_row + 1
	local skill = Widgets.SkillControl{
		cap = 0, desc = desc, id = id, index = index,
		max = 100, name = name, text = name, value = 0,
		pressed = function(self)
			local v = self:get_value_at(Client.cursor_pos)
			if v then self.cap = v end
			Skills:show(self.index)
			Skills:changed(self.index)
		end}
	clss.dict_id[id] = skill
	clss.dict_row[index] = skill
	clss.window.list:append_row(skill)
end

--- Sends the value of the currently selected skill to the server.
-- @param clss Skills class.
-- @param index Skill index.
Skills.changed = function(clss, index)
	local skill = clss.dict_row[index]
	if not skill then return end
	Network:send{packet = Packet(packets.SKILLS,
		"string", skill.id, "float", skill.cap)}
end

--- Shows s skill.
-- @param clss Skills class.
-- @param index Skill number.
Skills.show = function(clss, index)
	local skill = clss.dict_row[index]
	local species = Species:find{name = Player.species}
	local spec = species and species.skills[skill.id]
	clss.window.skill_name.text = skill.name
	clss.window.skill_desc.text = skill.desc
end

--- Toggles the visibility of the skill list.
-- @param clss Skills class.
Skills.toggle = function(clss)
	Gui.menus:open{widget = clss.window}
	--clss.window.floating = not clss.window.floating
end

--- Updates a skill.
-- @param clss Skills class.
-- @param id Skill id.
-- @param value Current value.
-- @param cap Custom cap.
Skills.update = function(clss, id, value, cap)
	local skill = clss.dict_id[id]
	if skill then
		local species = Species:find{name = Player.species}
		local spec = species and species.skills[skill.id]
		skill.value = value
		skill.cap = cap
		skill.max = spec and spec.max or 100
		if id == "health" then
			Gui.skill_health.cap = cap
			Gui.skill_health.value = value
		end
		if id == "willpower" then
			Gui.skill_mana.cap = cap
			Gui.skill_mana.value = value
		end
	end
end

------------------------------------------------------------------------------

Skills:init()
Skills:add("dexterity", "Dexterity",
	"Determines the effectiveness of your ranged attacks, as well as how " ..
	"fast you can move and react.")
Skills:add("health", "Health",
	"Determines how much damage your can withstand.")
Skills:add("intelligence", "Intelligence",
	"Determines what items you can craft and how effectively you can handle " ..
	"technology based items.")
Skills:add("perception", "Perception",
	"Determines how far you can see and how well you can handle precision " ..
	"weapons and items.")
Skills:add("strength", "Strength",
	"Determines your physical power and the effectiveness of your melee " ..
	"attacks.")
Skills:add("willpower", "Willpower",
	"Determines what spells you can cast and how effective they are.")
Skills:show(1)
