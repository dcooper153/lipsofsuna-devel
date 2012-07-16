Widgets.Hudcompass = Class(Widget)
Widgets.Hudcompass.class_name = "Widgets.Hudcompass"

--- Creates a new compass widget.
-- @param clss Compass class.
-- @return Compass widget.
Widgets.Hudcompass.new = function(clss)
	local self = Widget.new(clss)
	self.timer = 0
	return self
end

--- Updates the compass.
-- @param self Compass widget.
-- @param secs Seconds since the last update.
Widgets.Hudcompass.update = function(self, secs)
	self.timer = self.timer + secs
	if self.timer < 0.03 then return end
	self.timer = 0
	self.look_direction = math.pi - PlayerState.rotation_curr.euler[1]
	self.quest_direction = Operators.quests:get_compass_direction()
	self.quest_distance = Operators.quests:get_compass_distance()
	self.quest_height = Operators.quests:get_compass_height()
	self:reshaped()
end

Widgets.Hudcompass.reshaped = function(self)
	local w = self.width
	local h = self.height
	local mode = Program.video_mode
	self.offset = Vector(0, mode[2] - 80)
	self:canvas_clear()
	-- Add the background.
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {74,74},
		rotation = self.look_direction,
		rotation_center = Vector(38,37),
		source_image = "compass1",
		source_position = {42,2},
		source_tiling = {0,74,0,0,74,0}}
	-- Add the quest marker.
	if self.quest_direction then
		local frac = 0.6 + 0.4 * math.min(1,self.quest_distance/(50*Voxel.tile_size))
		self:canvas_image{
			dest_position = {32,44-33*frac},
			dest_size = {12,50},
			rotation = self.quest_direction + self.look_direction + math.pi,
			rotation_center = Vector(7,25),
			source_image = "compass1",
			source_position = {0,0},
			source_tiling = {0,12,0,0,50,0}}
	end
	-- Add the quest height offset.
	if self.quest_height then
		self:canvas_text{
			dest_position = {0,0},
			dest_size = {74,74},
			text = string.format("%+d", self.quest_height),
			text_alignment = {0.5,0.5},
			text_color = Theme.text_color_3,
			text_font = "tiny"}
	end
end
