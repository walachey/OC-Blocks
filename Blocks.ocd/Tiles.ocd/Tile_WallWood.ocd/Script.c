/*--- Amethyst ---*/

#include Lib_WallBuildingTile

local Name = "$Name$";
local Description = "$Description$";
local Collectible = 1;

local build_grid_x = 12;
local build_grid_y = 12;

local tile_size_x = 12;
local tile_size_y = 12;

protected func Hit(x, y)
{
	StonyObjectHit(x,y);
	return true;
}

local Components = {Wood = 1};
public func IsToolProduct() { return true; }