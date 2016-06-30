/*--- Amethyst ---*/

#include Lib_PlatformBuildingTile

local Name = "$Name$";
local Description = "$Description$";
local Collectible = 1;

local support;

func Construction()
{
	support = CreateArray(2);
	
	return _inherited();
}

func Constructed()
{
	BuildSupport();
	
	return _inherited();
}

func BuildSupport()
{
	if (FindObject(Find_AtPoint(-tile_size_x, 0), Find_Func("IsSolidBuildingTile")))
	{
		support[0] = CreateObject(PlatformSupportWood, 0, 0, GetController());
	}

	if (FindObject(Find_AtPoint(tile_size_x, 0), Find_Func("IsSolidBuildingTile")))
	{
		support[1] = CreateObject(PlatformSupportWood, 0, 0, GetController());
		support[1]->SetObjDrawTransform(-1000, 0, 0, 0, 1000);
	}
}

func Destruct()
{
	RemoveSupport();
	
	return _inherited();
}

func Destroy()
{
	RemoveSupport();
	
	return _inherited();
}

func RemoveSupport()
{
	for (var s in support)
	{
		if(s)
			s->RemoveObject();
	}
}

protected func Hit(x, y)
{
	StonyObjectHit(x,y);
	return true;
}


local Components = {Wood = 1};
public func IsToolProduct() { return true; }