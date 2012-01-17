Lighting = Class()
Lighting.class_name = "Lighting"

Lighting.init = function(self)
	self.light_global = Light{diffuse={1,1,1,1}, directional = true, equation={1.3,0,0}, shadow_casting = true}
	--, spot_cutoff = 1.5, spot_exponent = 127}
	self.light_spell = Light{diffuse={1,1,1,1}, equation={1.5,0,0.05}}
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

Lighting.set_light_spell = function(self, value)
	self.light_spell.enabled = value
end
