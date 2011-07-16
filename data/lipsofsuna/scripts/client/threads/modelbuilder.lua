require "system/class"
require "system/core"
require "system/math"
require "system/model"
require "system/model-editing"
require "common/bitwise"
require "common/faction"
require "common/feat"
require "common/model"
require "common/itemspec"
require "common/species"
require "content/armor"
require "content/enemies"
require "content/factions"
require "content/feats"
require "content/items"
require "content/models"
require "content/npcs"
require "content/species"
require "content/weapons"

-- Character meshes are kept in memory to ensure fast builds.
-- TODO: Could also cache morphed models for a short period.
Model.dict_name = {}

Model.find_or_load_lod = function(self, args)
	local m = Model:find_or_load(args)
	if not m then return end
	if not m.lod then
		m:calculate_lod()
		m.lod = true
	end
	return m
end

local build = function(args)
	local species = Species:find{name = args.species}
	if not species then return end
	-- Get the base meshes.
	local meshes = {skeleton = species.model}
	for k,v in pairs(species.models) do
		meshes[k] = v
	end
	-- Add the hair model.
	if args.hair_style and args.hair_style ~= "" then
		meshes.hair = args.hair_style
	end
	-- Add equipment models.
	if args.equipment then
		for slot,name in pairs(args.equipment) do
			local spec = Itemspec:find{name = name}
			if spec and spec.equipment_models then
				local models = spec:get_equipment_models(species.equipment_class or species.name, lod)
				if models then
					for k,v in pairs(models) do
						meshes[k] = v
					end
				end
			end
		end
	end
	-- Create the skeleton.
	local m = Model:find_or_load_lod{file = meshes.skeleton}
	m = m:copy()
	-- Add other meshes.
	local has_head = not args.beheaded
	local mesh_head = {eyes = true, head = true, hair = true}
	for k,v in pairs(meshes) do
		if k ~= "skeleton" and (has_head or not mesh_head[k]) then
			local tmp
			local ref = Model:find_or_load_lod{file = v}
			-- Face customization.
			if args.face_style and (string.match(k, ".*head.*") or string.match(k, ".*eye.*")) then
				tmp = ref:copy()
				if args.face_style[1] then
					if args.face_style[1] < 0.5 then
						tmp:morph("cheekbone small", 1 - 2 * args.face_style[1], ref)
					elseif args.face_style[1] > 0.5 then
						tmp:morph("cheekbone big", 2 * args.face_style[1] - 1, ref)
					end
				end
				if args.face_style[2] then tmp:morph("cheek small", 1 - args.face_style[2], ref) end
				if args.face_style[3] then tmp:morph("chin rough", args.face_style[3], ref) end
				if args.face_style[4] then tmp:morph("chin round", args.face_style[4], ref) end
				if args.face_style[5] then tmp:morph("chin small", 1 - args.face_style[5], ref) end
				if args.face_style[6] then tmp:morph("eye inner", args.face_style[6], ref) end
				if args.face_style[7] then tmp:morph("eye outer", args.face_style[7], ref) end
				if args.face_style[8] then tmp:morph("eye small", 1 - args.face_style[8], ref) end
				if args.face_style[9] then tmp:morph("face thin", 1 - args.face_style[9], ref) end
				if args.face_style[10] then tmp:morph("face wrinkle", args.face_style[10], ref) end
				if args.face_style[11] then tmp:morph("jaw wide", args.face_style[11], ref) end
				if args.face_style[12] then tmp:morph("nose dull", 1 - args.face_style[12], ref) end
			end
			-- Body customization.
			if args.body_style then
				if not tmp then tmp = ref:copy() end
				if args.body_style[1] then tmp:morph("hips wide", args.body_style[1], ref) end
				if args.body_style[2] then tmp:morph("limbs muscular", args.body_style[2], ref) end
				if args.body_style[3] then
					if args.body_style[3] < 0.5 then
						tmp:morph("torso small", 1 - 2 * args.body_style[3], ref)
					elseif args.body_style[3] > 0.5 then
						tmp:morph("torso big", 2 * args.body_style[3] - 1, ref)
					end
				end
				if args.body_style[4] then tmp:morph("torso thick", args.body_style[4], ref) end
				if args.body_style[5] then tmp:morph("waist thick", args.body_style[5], ref) end
			end
			-- Merge to the character model.
			m:merge(tmp or ref)
		end
	end
	-- Colorize materials.
	m:edit_material{match_shader = "hair", diffuse = args.hair_color}
	m:edit_material{match_shader = "skin", diffuse = args.skin_color,
		shader = species.skin_shader, textures = species.skin_textures}
	m:edit_material{match_texture = "eye1", diffuse = args.eye_color}
	-- Recalculate the bounding box.
	m:calculate_bounds()
	return m
end

while not Program.quit do
	-- Get a build order.
	local msg
	while true do
		msg = Program:pop_message()
		if msg then break end
		Program:wait(0.02)
		if Program.quit then return end
	end
	-- Build the model.
	local args = assert(loadstring("return " .. msg.string))()
	local mdl = args and build(args)
	-- Return the model.
	Program:push_message{name = msg.name, model = mdl}
end
