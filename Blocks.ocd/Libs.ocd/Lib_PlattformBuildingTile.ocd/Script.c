/**
	Lib_BuildingElement
	

	@author KKenny
*/

#include Lib_BuildingTile

local Name = "$Name$";
local Description = "$Description$";
local Plane = 55;

func MaxStackCount() { return 48; }
func InitialStackCount() { return 4; }

func IsPlattformBuildingTile() { return true; }
local tile_mode = TILE_MODE_HORIZONTAL_LINE;

func Initialize()
{
	SetHalfVehicleSolidMask(true);
	return _inherited();
}

func Constructed()
{
	SetSolidMask(0,0,10,10);
	SetHalfVehicleSolidMask(true);
	
	return _inherited();
}

func Destruct()
{
	SetSolidMask();
	return _inherited();
}


func BuildingCondition()
{
	if (FindObject(Find_AtPoint(), Find_NoContainer(), Find_Func("IsPlattformBuildingTile"), Find_Not(Find_Func("IsPreview")), Find_Exclude(this)))
		return false;

	if (VerticesStuckSemi() == GetVertexNum()+1)
		return false;

	if (FindObject(Find_OnLine(-tile_size_x/2-1, 0, tile_size_x/2+1, 0),
		Find_Exclude(this), Find_NoContainer(), Find_Or(Find_Func("IsSolidBuildingTile"), Find_Func("IsPlattformBuildingTile")), Find_Not(Find_Func("IsPreview"))))
		return true;
	
	return false;
}

func SpecialPreviewCondition()
{
	if (VerticesStuckSemi() == GetVertexNum()+1)
		return false;
	
	return _inherited();
}