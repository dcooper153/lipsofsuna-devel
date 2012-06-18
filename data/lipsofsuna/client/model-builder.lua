ModelBuilder = Class()
ModelBuilder.class_name = "ModelBuilder"

--- Builds the mesh for the given object.
-- @param clss ModelBuilder class.
-- @param object Object whose model to build and set.
-- @param args Model building arguments.
ModelBuilder.build = function(clss, object, args)
	-- Create or reuse the model merger.
	local merger = object.model_merger
	if not merger then
		merger = Merger()
		object.model_merger = merger
	end
	-- Get the base meshes.
	local meshes = {skeleton = args.spec.model}
	for k,v in pairs(args.spec.models) do
		meshes[k] = v
	end
	-- Override the head style.
	if args.head_style and args.head_style ~= "" then
		meshes["head"] = args.head_style
	end
	-- Add the hair model.
	if args.hair_style and args.hair_style ~= "" then
		meshes.hair = args.hair_style
	end
	-- Add equipment models.
	if args.equipment then
		for slot,name in pairs(args.equipment) do
			local spec = Itemspec:find{name = name}
			local models = spec and spec:get_equipment_models(args.spec.equipment_class or args.spec.name, lod)
			if models then
				for k,v in pairs(models) do
					if v ~= "" then meshes[k] = v end
				end
			end
		end
	end
	-- Remove worksafety stuff in the eyecandy mode.
	if args.nudity then
		meshes["lower_safe"] = nil
		meshes["upper_safe"] = nil
	end
	-- Remove the top if the character is sufficiently male.
	if args.body_style then
		local male = 0
		if args.body_style[3] and args.body_style[3] < 0.1 then male = male + 5 end -- breast size
		if args.body_style[4] and args.body_style[4] < 0.3 then male = male + 1 end -- hips wide
		if args.body_style[4] and args.body_style[4] > 0.6 then male = male - 5 end -- hips wide
		if args.body_style[6] and args.body_style[6] > 0.3 then male = male + 1 end -- torso wide
		if args.body_style[8] and args.body_style[8] > 0.5 then male = male + 1 end -- waist wide
		if male >= 7 then meshes["upper_safe"] = nil end
	end
	-- Remove the head if beheaded.
	if args.beheaded then
		for k,v in pairs{"eyes", "head", "hair"} do
			meshes[v] = nil
		end
	end
	-- Morph and merge the submodels.
	local model = Model:find_or_load{file = meshes["skeleton"]}
	if model then merger:add_model(model) end
	meshes["skeleton"] = nil
	for k,v in pairs(meshes) do
		clss:build_submesh(merger, k, v, args)
	end
	-- Colorize materials.
	local skin = args.skin_style and Actorskinspec:find{name = args.skin_style}
	merger:replace_material{match_material = "animeye1", diffuse = args.eye_color}
	merger:replace_material{match_material = "animhair1", diffuse = args.hair_color}
	if skin then
		merger:replace_material{match_material = "animskin1", diffuse = args.skin_color,
			material = skin.material, textures = skin.textures}
	else
		merger:replace_material{match_material = "animskin1", diffuse = args.skin_color,
			material = args.spec.skin_material, textures = args.spec.skin_textures}
	end
	-- Queue the build.
	merger:finish()
end

--- Builds a submesh.
-- @param clss ModelBuilder class.
-- @param merger Model merger.
-- @param name Submesh name.
-- @param file Filename of the model.
-- @param args Model building arguments.
ModelBuilder.build_submesh = function(clss, merger, name, file, args)
	-- Load the model.
	local ref = Model:find_or_load{file = file}
	local morph = {}
	local add = function(name, value)
		if not value then return end
		table.insert(morph, name)
		table.insert(morph, value)
	end
	-- Face customization.
	if args.face_style and (string.match(name, ".*head.*") or string.match(name, ".*eye.*")) then
		add("cheekbone small", args.face_style[1])
		add("cheek small", args.face_style[2])
		add("chin sharp", args.face_style[3])
		add("chin small", args.face_style[4])
		add("eye inner", args.face_style[5])
		add("eye near", args.face_style[6])
		add("eye outer", args.face_style[7])
		add("eye small", args.face_style[8])
		add("face wrinkle", args.face_style[9])
		add("jaw straight", args.face_style[10])
		add("jaw wide", args.face_style[11])
		add("lips protrude", args.face_style[12])
		add("mouth wide", args.face_style[13])
		add("nose dull", args.face_style[14])
		add("nose up", args.face_style[15])
	end
	-- Body customization.
	if args.body_style then
		add("arms muscular", args.body_style[1])
		add("body thin", args.body_style[2])
		if args.body_style[3] then
			if args.body_style[3] < 0.5 then
				add("breast small", 1 - 2 * args.body_style[3])
			elseif args.body_style[3] > 0.5 then
				add("breast big", 2 * args.body_style[3] - 1)
			end
		end
		add("hips wide", args.body_style[4])
		add("legs muscular", args.body_style[5])
		add("torso wide", args.body_style[6])
		add("waist fat", args.body_style[7])
		add("waist wide", args.body_style[8])
		if args.body_style[9] then
			add("shoulder thin", 1 - args.body_style[9])
		end
	end
	-- Morph and merge.
	merger:add_model_morph(ref, morph)
end
