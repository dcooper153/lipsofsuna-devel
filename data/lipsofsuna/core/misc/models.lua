-- Modelspecs are only needed if you want to override the materials of the
-- model without re-exporting.

local ModelSpec = require("core/specs/model")

for k,v in pairs({"bitingbark1", "bloodworm1", "boomerang1", "devorahair1", "dragon1", "lizardman1", "lizardman2"}) do
	ModelSpec{name = v, edit_materials = {
		{match_shader = "default", material = "animdiff1"},
		{match_shader = "normalmap", material = "animskin1"},
		{match_shader = "eye", material = "animeye1"},
		{match_shader = "fur", material = "animdiff1"},
		{match_shader = "skin", material = "animskin1"},
		{match_shader = "animdiff", material = "animdiff1"},
		{match_shader = "animdiffnorm", material = "animskin1"},
		{match_shader = "animdiffnorm1", material = "animskin1"}
		}}
end

ModelSpec{name = "bloodworm2", replace_shaders = {default = "animdiff", glass = "animslime", normalmap = "animdiffnorm"}}
ModelSpec{name = "slime2", replace_shaders = {default = "animdiff", glass = "animslime", normalmap = "animdiffnorm"}}
