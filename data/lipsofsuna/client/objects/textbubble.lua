TextBubble = Class()
TextBubble.dict = {}
TextBubble.dict_id = {}

--- Creates a new text bubble.
-- @param clss TextBubble class.
-- @param args Arguments.<ul>
--   <li>font: Font name.</li>
--   <li>font_color: Font color table.</li>
--   <li>life: Life time in seconds.</li>
--   <li>node: Parent node or nil.</li>
--   <li>object: Parent object or nil.</li>
--   <li>position: Position in world space.</li>
--   <li>text: Text effect string.</li>
--   <li>velocity: Velocity vector.</li></ul>
-- @return TextBubble
TextBubble.new = function(clss, args)
	-- Create the text widget.
	local life = args.life or 10
	local fade = args.fade or 0
	local widget = Widgets.Label{
		text = args.text, color = args.text_color,
		font = args.text_font, width_request = 150, halign = 0.5,
		life = life, fade = fade,
		velocity = args.velocity, motion = Vector()}
	-- Check for an existing bubble.
	if args.object then
		local self = clss.dict_id[args.object.id]
		if self then
			table.insert(self.widgets, widget)
			self:transform()
			Gui.scene:add_child(widget)
			return self
		end
	end
	-- Initialize self.
	local self = Class.new(clss, args)
	self.offset = self.position or Vector()
	-- Add the text widget.
	self.widgets = {widget}
	self:transform()
	Gui.scene:add_child(widget)
	-- Register for updates.
	TextBubble.dict[self] = true
	if self.object then
		TextBubble.dict_id[self.object.id] = self
	end
	return self
end

TextBubble.disable = function(self)
	if self.object then
		TextBubble.dict_id[self.object.id] = nil
	end
	TextBubble.dict[self] = nil
	for k,v in pairs(self.widgets) do
		v:detach()
	end
	self.widgets = nil
end

TextBubble.transform = function(self)
	if self.object then
		local p = node and self.object:find_node{name = self.node}
		if p then
			self.position = self.offset + self.object.position + self.object.rotation * p
		else
			self.position = self.offset + self.object.position
		end
	end
end

TextBubble.update = function(self, secs)
	-- Remove if the object disappeared.
	if self.object and not self.object.realized then
		self:disable()
		return
	end
	-- Remove expired text widgets.
	local n = #self.widgets
	for i = 1,n do
		local v = self.widgets[n - i + 1]
		v.life = v.life - secs
		if v.life < 0 then
			v:detach()
			table.remove(self.widgets, n - i + 1)
		end
	end
	-- Remove if no more text widgets left.
	n = #self.widgets
	if n == 0 then
		self:disable()
		return
	end
	-- Update text motion.
	for i = 1,n do
		local v = self.widgets[n - i + 1]
		if v.velocity then
			v.motion = v.motion + v.velocity * secs
		end
	end
	-- Calculate the position.
	self:transform()
	local p = Render:project(self.position)
	p.x = p.x - 75
	-- Move the text widgets.
	for i = 1,n do
		local v = self.widgets[n - i + 1]
		if v.life < v.fade then
			local c = v.color
			v.color = {c[1], c[2], c[3], v.life / v.fade}
		end
		v.offset = p + v.motion
		if not v.velocity then
			p = Vector(p.x, p.y - v.height - 5)
		end
	end
end
