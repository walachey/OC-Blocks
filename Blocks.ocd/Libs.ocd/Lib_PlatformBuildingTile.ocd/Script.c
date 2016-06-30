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

func IsPlatformBuildingTile() { return true; }
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
	if (FindObject(Find_AtPoint(), Find_NoContainer(), Find_Func("IsPlatformBuildingTile"), Find_Not(Find_Func("IsPreview")), Find_Exclude(this)))
		return false;

	if (VerticesStuckSemi() == GetVertexNum()+1)
		return false;

	if (FindObject(Find_OnLine(-tile_size_x/2-1, 0, tile_size_x/2+1, 0),
		Find_Exclude(this), Find_NoContainer(), Find_Or(Find_Func("IsSolidBuildingTile"), Find_Func("IsPlatformBuildingTile")), Find_Not(Find_Func("IsPreview"))))
		return true;
	
	return false;
}

func SpecialPreviewCondition()
{
	if (VerticesStuckSemi() == GetVertexNum()+1)
		return false;
	
	return _inherited();
}

private func Destroy()
{
	SetCategory(C4D_None);
	
	var color = PV_Random(50, 100, nil, 1);
	var particles = 
	{
		Size = PV_KeyFrames(0, 0, 0, 100, PV_Random(5, 8), 1000, 3),
		R = color, G = color, B = color,
		Alpha = PV_Linear(255, 0),
		ForceY = PV_Gravity(100),
		CollisionVertex = 0,
		Rotation = PV_Random(360)
	};
	CreateParticle("WoodChip", PV_Random(-build_grid_x/2, +build_grid_x/2), PV_Random(-build_grid_y/2, +build_grid_y/2), PV_Random(-2, 2), PV_Random(-2, 2), PV_Random(10, 60), particles, 20);
	RemoveObject();
} 

public func CheckSupport()
{
	this.already_found = true;
	var has_support = false;
	var neighbour_tiles = [this];
	for (var i = 0; i < GetLength(neighbour_tiles); ++i)
	{
		var current = neighbour_tiles[i];
		if (!current) continue;
		
		for (var neighbour in current->GetNeighbours(true))
		{
			neighbour.already_found = true;
			PushBack(neighbour_tiles, neighbour);
			
			if (neighbour->FindObject(Find_Or(Find_OnLine(-tile_size_x/2-1, 0, tile_size_x/2+1, 0), Find_OnLine(0, -tile_size_y/2-1, 0, tile_size_y/2+2)),
									 Find_Property("is_constructed"), Find_Func("IsPlatformBuildingTile")))
			{
				has_support = true;
				break;
			} 
		}
		
		if (has_support) break;
	}
	
	for (var neighbour in neighbour_tiles)
	{
		if (!neighbour) continue;
		
		if (!has_support)
		{
			neighbour.no_propagation = true;
			neighbour->Destroy();
		}
		else
		{
			neighbour.already_found = nil;
		}
	}
}