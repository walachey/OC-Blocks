/**
	Tile_Conveyor
	

	@author 
*/

#include Lib_PipeBuildingTile

local Name = "$Name$";
local Description = "$Description$";
local Collectible = 1;
local Plane = 15;

local IsConveyorBuildingTile = true;
local TileKindPropertyName = "IsConveyorBuildingTile"; 

protected func Hit(x, y)
{
	StonyObjectHit(x,y);
	return true;
}

func Constructed()
{
	_inherited();
	
	AddTimer("CheckObjects", 60 + Random(20));
}

public func Destruct()
{
	RemoveTimer("CheckObjects");
	return inherited(...);
}

private func CheckObjects()
{
	var rect = Find_AtRect(-build_grid_x / 2, -build_grid_y / 2, build_grid_x, build_grid_y * 4);
	var obj = FindObject(rect, Find_Category(C4D_Object), 
		Find_Not(Find_OCF(OCF_HitSpeed1)), Find_NoContainer(), Find_Not(Find_Property("is_being_moved_by_conveyor")),
		Find_PathFree(this)
		);
	if (!obj) return;
	if (FindObject(rect, Find_ID(Tile_ConveyorArmHook))) return;
	
	BeginTransport(obj);
}

private func BeginTransport(object obj)
{
	var base = CreateObject(Tile_ConveyorArmBase, BoundBy(obj->GetX() - GetX(), -build_grid_x / 2, +build_grid_x / 2), 0, NO_OWNER);
	base.hook->MoveToGrab(obj);
	base.home_block = this;
}

public func IsTarget(object origin, object payload)
{
	if (FindObject(Find_OnLine(0, 0, 0, 40), Find_ID(Lorry), Find_PathFree(this))) return true;
	return false;
}

local Components = {Wood = 1};
public func IsToolProduct() { return true; }
