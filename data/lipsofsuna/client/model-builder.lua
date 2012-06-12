ModelBuilder = Class()
ModelBuilder.class_name = "ModelBuilder"

ModelBuilder.build = function(clss, object, args)
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
	-- Create the models.
	local models = {}
	for k,v in pairs(meshes) do
		if k == "skeleton" then
			models[k] = Model:find_or_load{file = v}
		else
			models[k] = clss:build_submesh(k, v, args)
		end
	end
	-- Remove unused models.
	if object.used_models then
		for k,v in pairs(object.used_models) do
			if not models[k] then
				object:remove_model(v)
				object.used_models[k] = nil
			end
		end
	else
		object.used_models = {}
		object:set_model(nil)
	end
	-- Add new models.
	for k,v in pairs(models) do
		v:changed()
		local old = object.used_models[k]
		if old then
			object:replace_model(old, v)
		else
			object:add_model(v)
		end
		object.used_models[k] = v
	end
	return m
end

ModelBuilder.build_submesh = function(clss, name, file, args)
	local tmp
	local ref = Model:find_or_load{file = file}
	-- Face customization.
	if args.face_style and (string.match(name, ".*head.*") or string.match(name, ".*eye.*")) then
		tmp = ref:copy()
		if args.face_style[1] then tmp:morph("cheekbone small", args.face_style[1], ref) end
		if args.face_style[2] then tmp:morph("cheek small", args.face_style[2], ref) end
		if args.face_style[3] then tmp:morph("chin sharp", args.face_style[3], ref) end
		if args.face_style[4] then tmp:morph("chin small", args.face_style[4], ref) end
		if args.face_style[5] then tmp:morph("eye inner", args.face_style[5], ref) end
		if args.face_style[6] then tmp:morph("eye near", args.face_style[6], ref) end
		if args.face_style[7] then tmp:morph("eye outer", args.face_style[7], ref) end
		if args.face_style[8] then tmp:morph("eye small", args.face_style[8], ref) end
		if args.face_style[9] then tmp:morph("face wrinkle", args.face_style[9], ref) end
		if args.face_style[10] then tmp:morph("jaw straight", args.face_style[10], ref) end
		if args.face_style[11] then tmp:morph("jaw wide", args.face_style[11], ref) end
		if args.face_style[12] then tmp:morph("lips protrude", args.face_style[12], ref) end
		if args.face_style[13] then tmp:morph("mouth wide", args.face_style[13], ref) end
		if args.face_style[14] then tmp:morph("nose dull", args.face_style[14], ref) end
		if args.face_style[15] then tmp:morph("nose up", args.face_style[15], ref) end
	end
	-- Body customization.
	if args.body_style then
		if not tmp then tmp = ref:copy() end
		if args.body_style[1] then tmp:morph("arms muscular", args.body_style[1], ref) end
		if args.body_style[2] then tmp:morph("body thin", args.body_style[2], ref) end
		if args.body_style[3] then
			if args.body_style[3] < 0.5 then
				tmp:morph("breast small", 1 - 2 * args.body_style[3], ref)
			elseif args.body_style[3] > 0.5 then
				tmp:morph("breast big", 2 * args.body_style[3] - 1, ref)
			end
		end
		if args.body_style[4] then tmp:morph("hips wide", args.body_style[4], ref) end
		if args.body_style[5] then tmp:morph("legs muscular", args.body_style[5], ref) end
		if args.body_style[6] then tmp:morph("torso wide", args.body_style[6], ref) end
		if args.body_style[7] then tmp:morph("waist fat", args.body_style[7], ref) end
		if args.body_style[8] then tmp:morph("waist wide", args.body_style[8], ref) end
		if args.body_style[9] then tmp:morph("shoulder thin", 1 - args.body_style[9], ref) end
	end
	-- Colorize materials.
	-- TODO: Copying the model could perhaps be avoided in some cases.
	tmp = tmp or ref:copy()
	local skin = args.skin_style and Actorskinspec:find{name = args.skin_style}
	tmp:edit_material{match_material = "animeye1", diffuse = args.eye_color}
	tmp:edit_material{match_material = "animhair1", diffuse = args.hair_color}
	if skin then
		tmp:edit_material{match_material = "animskin1", diffuse = args.skin_color,
			material = skin.material, textures = skin.textures}
	else
		tmp:edit_material{match_material = "animskin1", diffuse = args.skin_color,
			material = args.spec.skin_material, textures = args.spec.skin_textures}
	end
	return tmp
end
