for k,v in pairs(File:scan_directory("scripts/content/patterns")) do
	require("content/patterns/" .. string.gsub(v, "([^.]*).*", "%1"))
end

Pattern{
	name = "corridor1",
	categories = {"corridor"},
	tiles = {
		{-1,-2,-1,nil,3,3,1},
		{-2,-1,-2,nil,5,5,3},
		{-1,2,-1,nil,3,3,1}}}

Pattern{
	name = "corridorx1",
	categories = {"corridorx"},
	tiles = {
		{0,-1,-2,nil,0,2,0},
		{0,-2,-1,nil,0,5,2},
		{0,-1,2,nil,0,2,0}}}

Pattern{
	name = "corridory1",
	categories = {"corridory"},
	tiles = {
		{-1,0,-2,nil,2,0,0},
		{-2,0,-1,nil,5,0,2},
		{-1,0,2,nil,2,0,0}}}

Pattern{
	name = "corridorz1",
	categories = {"corridorz"},
	tiles = {
		{-1,-2,0,nil,2,0,0},
		{-2,-1,0,nil,5,2,0},
		{-1,2,0,nil,2,0,0}}}
