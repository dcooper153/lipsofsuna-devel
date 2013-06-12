--- Builds multipart or morphed character models.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module character.model_builder
-- @alias ModelBuilder

local Class = require("system/class")
local Color = require("system/color")
local Model = require("system/model")
local ModelMerger = require("system/model-merger")

--- Builds multipart or morphed character models.
-- @type ModelBuilder
local ModelBuilder = Class("ModelBuilder")

--- Builds the mesh for the given actor.
-- @param clss ModelBuilder class.
-- @param object Object whose model to build.
ModelBuilder.build_for_actor = function(clss, object)
	if not object.spec then return end
	if not object.spec.models then return end
	-- Create the equipment list.
	local equipment = {}
	for k,v in pairs(object.inventory.equipped) do
		local item = object.inventory:get_object_by_index(v)
		equipment[k] = item.spec.name
	end
	-- Create or reuse the model merger.
	local merger = object.model_merger
	if not merger then
		merger = ModelMerger()
		object.model_merger = merger
	end
	-- Build the character model in a separate thread.
	-- The result is handled in the tick handler in event.lua.
	clss:build_with_merger(merger, {
		beheaded = object:get_beheaded(),
		body_scale = object.body_scale,
		body_style = object.body_style,
		equipment = equipment,
		eye_color = Color:ubyte_to_float(object.eye_color),
		eye_style = object.eye_style,
		face_style = object.face_style,
		hair_color = Color:ubyte_to_float(object.hair_color),
		hair_style = object.hair_style,
		head_style = object.head_style,
		nudity = Client.options.nudity_enabled,
		skin_color = Color:ubyte_to_float(object.skin_color),
		skin_style = object.skin_style,
		spec = object:get_spec()})
end

--- Builds the mesh for the given object.
-- @param clss ModelBuilder class.
-- @param merger Model merger to use.
-- @param args Model building arguments.
ModelBuilder.build_with_merger = function(clss, merger, args)
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
	local textures = {}
	if args.equipment then
		for slot,name in pairs(args.equipment) do
			local spec = Itemspec:find{name = name}
			if spec then
				local models = spec:get_equipment_models(args.spec.equipment_class or args.spec.name, lod)
				if models then
					for k,v in pairs(models) do
						if v ~= "" then
							meshes[k] = v
						else
							meshes[k] = nil
						end
					end
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
	local model = Main.models:find_by_name(meshes["skeleton"])
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
	local ref = Main.models:find_by_name(file)
	local morph = {}
	local add = function(spec, value)
		-- Validate the slider.
		if not value then return end
		local num = #spec.shape_keys
		if num == 0 then return end
		-- Scale and invert the value.
		value = math.min(math.max(value / 255, 0), 1)
		if spec.invert then value = 1 - value end
		-- Add the shape key.
		if num == 1 then
			-- Single with the default in the beginning of the chain.
			table.insert(morph, spec.shape_keys[1])
			table.insert(morph, value)
		elseif num == 2 then
			-- Dual with the default between the two keys.
			if value < 0.5 then
				table.insert(morph, spec.shape_keys[1])
				table.insert(morph, 1 - value * 2)
			else
				table.insert(morph, spec.shape_keys[2])
				table.insert(morph, value * 2 - 1)
			end
		else
			-- TODO
			val = val * num
			local key = math.min(math.floor(val) + 1, num)
			table.insert(morph, spec.sliders[key])
			table.insert(morph, val - key)
		end
	end
	-- Face customization.
	if args.face_style and (string.match(name, ".*head.*") or string.match(name, ".*eye.*")) then
		for k,spec in ipairs(ChargenSliderSpec:find_by_category("face")) do
			local field = args[spec.field_name]
			if field then
				add(spec, field[spec.field_index])
			end
		end
	end
	-- Body customization.
	if args.body_style then
		for k,spec in ipairs(ChargenSliderSpec:find_by_category("body")) do
			local field = args[spec.field_name]
			if field then
				add(spec, field[spec.field_index])
			end
		end
	end
	-- Morph and merge.
	merger:add_model_morph(ref, morph)
end

return ModelBuilder
