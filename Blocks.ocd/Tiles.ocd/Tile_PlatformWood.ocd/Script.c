/*--- Amethyst ---*/

#include Lib_PlatformBuildingTile

local Name = "$Name$";
local Description = "$Description$";
local Collectible = 1;

local supportID = PlatformSupportWood;


protected func Hit(x, y)
{
	StonyObjectHit(x,y);
	
	return true;
}


local Components = {Wood = 1};
public func IsToolProduct() { return true; }