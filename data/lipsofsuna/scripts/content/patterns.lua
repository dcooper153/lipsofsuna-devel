require "content/patterns/charasgrove1"
require "content/patterns/charasillusion1"
require "content/patterns/dungeon1"
require "content/patterns/dungeon2"
require "content/patterns/dungeon3"
require "content/patterns/dungeon4"
require "content/patterns/erinyeslair1"
require "content/patterns/erinyesportal1"
require "content/patterns/lips1"
require "content/patterns/midguard1"
require "content/patterns/nature1"
require "content/patterns/nature2"
require "content/patterns/noemi1"
require "content/patterns/ocean1"
require "content/patterns/portaloflips1"
require "content/patterns/portalofmidguard1"
require "content/patterns/sanctuary1"
require "content/patterns/silverspring1"
require "content/patterns/volcano1"

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
require "content/patterns/brigandhq1"
