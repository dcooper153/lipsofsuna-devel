Influencespec{name = "push", func = function(feat, info, args, value)
	if not args.target then return end
	if args.target:face_point() then args.target:face_point{point = args.attacker.position} end
	args.target:set_movement(value)
end} 
