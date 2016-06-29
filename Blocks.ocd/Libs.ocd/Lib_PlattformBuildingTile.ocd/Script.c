/**
	Lib_BuildingElement
	

	@author KKenny
*/

#include Lib_BuildingTile

local Name = "$Name$";
local Description = "$Description$";
local Plane = 5;

func IsPlattformBuildingTile() { return true; }
local tile_mode = TILE_MODE_SINGLE;

func Construction()
{
	SetHalfVehicleSolidMask(true);
}

func Constructed()
{
	SetSolidMask(0,0,10,10);

	return _inherited();
}

func Destruct()
{
	SetSolidMask();
}


func BuildingCondition()
{
	if (FindObject(Find_AtPoint(), Find_NoContainer(), Find_Func("IsPlattformBuildingTile"), Find_Not(Find_Func("IsPreview")), Find_Exclude(this)))
		return false;

	if (VerticesStuckSemi() == GetVertexNum()+1)
		return false;

	if (FindObject(Find_Exclude(this), Find_NoContainer(), Find_Func("IsBuildingTile"), Find_Not(Find_Func("IsPreview")),
		Find_Or(Find_OnLine(-tile_size_x/2-1, 0, tile_size_x/2+1, 0), Find_OnLine(0, -tile_size_y/2-1, 0, tile_size_y/2+2))))
		return true;
	
	return false;
}

func SpecialPreviewCondition()
{
	if (VerticesStuckSemi() == GetVertexNum()+1)
		return false;
	
	return _inherited();
}