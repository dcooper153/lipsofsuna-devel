local Class = require("system/class")

local Lighting = Class()
Lighting.class_name = "Lighting"

Lighting.new = function(clss)
	local self = Class.new(clss)
	self.light_global = Light{diffuse={1,1,1,1}, directional = true, equation={1.9,0,0},
	 shadow_casting = true}
	self.light_spell = Light{diffuse={1,1,1,1}, equation={1.5,0,0.05}}
	self.light_spell_timer = 0
	Render.scene_ambient = {0.5,0.5,0.5}
	-- Set the skybox.
	Render.skybox = "skybox1"
	return self
end

Lighting.update = function(self, secs)
	-- Update the global light.
	if self.dungeon_mode then
		self.light_global.rotation = Quaternion(-0.68,0,0,0.82)
	else
		self.light_global.rotation = Quaternion(-0.16,-0.84,-0.32,0.42)
	end
	self.light_global.enabled = true
	-- Update the light spell position.
	if Client.player_object and self.light_spell.enabled then
		local p = Client.player_object.position
		self.light_spell_timer = self.light_spell_timer + secs
		p.x = p.x + math.sin(self.light_spell_timer)
		p.y = p.y + math.cos(0.9*self.light_spell_timer)
		p.z = p.z + math.cos(-0.8*self.light_spell_timer)
		local r = Client.player_object.rotation
		self.light_spell.position = p + r * Vector(0,1.7,-1.5)
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
	if value then
		self.light_global.equation = {6,0,0}
		Render.scene_ambient = {0.3,0.3,0.3}
	else
		self.light_global.equation = {1.9,0,0}
		Render.scene_ambient = {0.5,0.5,0.5}
	end
	-- Reconfigure the visibility of the skybox.
	Render.skybox = value and "" or "skybox1"
end

Lighting.set_light_spell = function(self, value)
	self.light_spell.enabled = value
end

return Lighting
