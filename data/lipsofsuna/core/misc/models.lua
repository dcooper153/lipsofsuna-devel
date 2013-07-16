-- Modelspecs are only needed if you want to override the materials of the
-- model without re-exporting.

for k,v in pairs({"aerarmplate1", "aerbra1", "aerbloomers1", "aerbloomerstop1", "aerchinadress1", "aerfedora1", "aerheadband1", "aerleatherchest1", "aerleatherskirt1", "aerlegarmor2", "aerleggings1", "aermale1arms1", "aermale1boots1", "aermale1chest1", "aermale1eyes1", "aermale1feet1", "aermale1hair1", "aermale1hands1", "aermale1head1", "aermale1leatherarms2", "aermale1leatherchest1", "aermale1leatherchest2", "aermale1leatherhead2", "aermale1leatherlegs1", "aermale1leatherlegs2", "aermale1legs1", "aermale1", "aermale1pants1", "aermale1shirt1", "aershorthair1", "aerskirt1", "aerupperarmor1", "bitingbark1", "bloodworm1", "boomerang1", "devorahair1", "dragon1", "krakenchinadress1", "lizardman1", "lizardman2", "panties1", "panties2", "plaguerat1", "seirei1", "smallrat1", "stoneimp1", "stoneimp2", "trogfem1", "trogfemarmnude1", "trogfemhead1", "trogfemlegnude1", "trogfemlonghair1", "trogfemlowernude1", "trogfemrags1", "trogfemshorthair1", "trogfemuppernude1", "wyrmhead1", "wyrmlowernude1"}) do
	Modelspec{name = v, edit_materials = {
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

Modelspec{name = "bloodworm2", replace_shaders = {default = "animdiff", glass = "animslime", normalmap = "animdiffnorm"}}
Modelspec{name = "slime2", replace_shaders = {default = "animdiff", glass = "animslime", normalmap = "animdiffnorm"}}
