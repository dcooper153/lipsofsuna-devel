Lighting = Class()
Lighting.class_name = "Lighting"

Lighting.init = function(self)
	self.light_global = Light{diffuse={1,1,1,1}, directional = true, equation={1.9,0,0},
	 shadow_casting = true}
	--, spot_cutoff = 1.5, spot_exponent = 127}
	self.light_spell = Light{diffuse={1,1,1,1}, equation={1.5,0,0.05}}
	Render.scene_ambient = {0.5,0.5,0.5}
	-- Set the skybox.
	Render.skybox = "skybox1"
end

Lighting.update = function(self, secs)
	-- Update the global light.
	self.light_global.rotation = Quaternion(-0.16,-0.84,-0.32,0.42)
	self.light_global.enabled = true
	-- Update the light spell.
	if Client.player_object and self.light_spell.enabled then
		local p = Client.player_object.position
		local r = Client.player_object.rotation
		self.light_spell.position = p + r * Vector(0,2,-1.5)
		--self.light_spell.rotation = Quaternion{dir = p - self.light_spell.position, up = Vector(0, 1)}
	end
end

--- Changes the lighting to be overworld or dungeon-like.
-- @param self Lighting class.
-- @param value Boolean, true for dungeon, false for overworld.
Lighting.set_dungeon_mode = function(self, value)
	-- Update the mode flag.
	if self.dungeon_mode == value then return end
	self.dungeon_mode = value
	-- Reconfigure the brightness of the global light.
	self.light_global.equation = value and {2.1,0,0} or {1.9,0,0}
	Render.scene_ambient = value and {0.2,0.2,0.2} or {0.5,0.5,0.5}
	-- Reconfigure the visibility of the skybox.
	Render.skybox = value and "" or "skybox1"
end

Lighting.set_light_spell = function(self, value)
	self.light_spell.enabled = value
end
