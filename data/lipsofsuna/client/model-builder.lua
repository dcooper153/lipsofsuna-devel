ModelBuilder = Class()
ModelBuilder.class_name = "ModelBuilder"

--- Builds the mesh for the given object.
-- @param clss ModelBuilder class.
-- @param object Object whose model to build and set.
-- @param args Model building arguments.
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
	--
	-- Due to body morphing and skin customization, each submodel
	-- generally needs to be copied and modified before it can be
	-- added to the object.
	local models = {}
	for k,v in pairs(meshes) do
		if k == "skeleton" then
			models[k] = Model:find_or_load{file = v}
		else
			models[k] = clss:build_submesh(k, v, args)
		end
	end
	-- Set the skeleton.
	--
	-- This needs to be set separately at the start so that the object
	-- will get the right collision shape. Changing the skeleton will
	-- make the object disappear until built so we only do it if the
	-- skeleton is different.
	local s1 = object.used_models and object.used_models["skeleton"]
	local s2 = models["skeleton"]
	if not s1 or not s2 or s1.file ~= s2.file then
		object.used_models = {}
		object.model = s2
	end
	-- Remove unused models.
	for k,v in pairs(object.used_models) do
		if not models[k] then
			if Object.remove_model then
				object:remove_model(v)
			end
			object.used_models[k] = nil
		end
	end
	-- Add new models.
	for k,v in pairs(models) do
		if k ~= "skeleton" then
			v:changed()
			local old = object.used_models[k]
			if old then
				if Object.replace_model then
					object:replace_model(old, v)
				end
			else
				if Object.add_model then
					object:add_model(v)
				end
			end
			object.used_models[k] = v
		end
	end
end

--- Builds a submesh.
-- @param clss ModelBuilder class.
-- @param name Submesh name.
-- @param file Filename of the model.
-- @param args Model building arguments.
ModelBuilder.build_submesh = function(clss, name, file, args)
	local tmp
	local ref = Model:find_or_load{file = file}
	-- Face customization.
	local morph = {}
	local add = function(name, value)
		if not value then return end
		table.insert(morph, name)
		table.insert(morph, value)
	end
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
	-- Colorize materials.
	-- TODO: Copying the model could perhaps be avoided in some cases.
	tmp = ref:morph_copy(morph)
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
