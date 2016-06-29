/**
	Lib_PipeBuildingTile
	

	@author 
*/

#include Lib_BuildingTile

local Name = "$Name$";
local Description = "$Description$";


local tile_mode = TILE_MODE_VERTICAL_LINE;
local Plane = 15;

local IsPipeBuildingTile = true;

// Needs to be overwritten.
local TileKindPropertyName = "IsPipeBuildingTile";
// Neighbours as a proplist (left/right/up/bottom).
local neighbours;
// Neighbours in 3x3 array. MIGHT not be up to date at all times.
local last_neighbours_as_matrix;
// Amount of current neighbours.
local neighbour_count = 0;

public func Construction()
{
	return inherited();
}

public func Constructed()
{
	_inherited();
	
	neighbour_count = 0;
	neighbours = {left = nil, right = nil, up = nil, bottom = nil};
		
	var current_neighbours = GetNeighboursAsMatrix();
	AddNeighbour("left", current_neighbours[0][1]);
	AddNeighbour("right", current_neighbours[2][1]);
	AddNeighbour("up", current_neighbours[1][0]);
	AddNeighbour("bottom", current_neighbours[1][2]);
	this->UpdateGraphics(current_neighbours);
	
	for (var neighbour in [neighbours.left, neighbours.right, neighbours.up, neighbours.bottom])
	{
		if (!neighbour) continue;
		neighbour->UpdateGraphics(neighbour->GetNeighboursAsMatrix());
	}
}

public func BuildingCondition()
{
	if (FindObject(Find_AtPoint(), Find_Property("is_constructed"), Find_Not(Find_Or(Find_Func("IsWallBuildingTile"), Find_Func("IsPillarBuildingTile"))), Find_Func("IsBuildingTile"), Find_Exclude(this)))
		return false;

	if (VerticesStuckSemi() == GetVertexNum()+1)
		return false;
	
	if (FindObject(Find_Or(Find_OnLine(-tile_size_x/2-1, 0, tile_size_x/2+1, 0), Find_OnLine(0, -tile_size_y/2-1, 0, tile_size_y/2+2)),
		Find_Exclude(this), Find_Property("is_constructed"), Find_Property(TileKindPropertyName)))
		return true;
	
	if (FindObject(Find_AtPoint(), Find_Property("is_constructed"), Find_Func("IsPillarBuildingTile"), Find_Exclude(this)))
		return true;
	
	return false;
}

func Destruct()
{
	_inherited();
	OnBecomeUnstable();
}

private func Destroy()
{
	SetCategory(C4D_None);
	OnBecomeUnstable();
	
	var particles = 
	{
		Size = PV_KeyFrames(0, 0, 0, 100, PV_Random(0, 2), 1000, 3),
		R = PV_Random(100, 150, nil, 1), G = PV_Random(100, 150, nil, 1), B = PV_Random(100, 150, nil, 1),
		Alpha = PV_Linear(255, 0),
		ForceY = PV_Gravity(200),
		CollisionVertex = 0,
		Stretch = PV_Random(2000, 4000),
		Rotation = PV_Random(-10, 10)
	};
	CreateParticle("SmokeDirty", PV_Random(-build_grid_x, +build_grid_x), PV_Random(-build_grid_y, +build_grid_y), PV_Random(-15, 15), PV_Random(-2, 2), PV_Random(10, 30), particles, 50);
	Sound("Hits::Materials::Rock::Rockfall*");
	RemoveObject();
} 

public func OnBecomeUnstable()
{
	if (this && this.no_propagation) return;
}

private func AddNeighbour(string direction, object neighbour, bool no_recursion)
{
	if (!neighbour && neighbours[direction])
		--neighbour_count;
	else if (neighbour && !neighbours[direction])
		++neighbour_count;
		
	neighbours[direction] = neighbour;
	if (!neighbour) return;

	if (!no_recursion)
	{
		var mapping = {left = "right", right = "left", up = "bottom", bottom = "up"};
		neighbour->AddNeighbour(mapping[direction], this, true);
		this->OnAddedNeighbour(direction, neighbour);
	}
}

private func GetNeighboursAsMatrix(bool ignore_cycles)
{
	var blocks = [[nil, nil, nil], [nil, nil, nil], [nil, nil, nil]];
	var x_pos = [-1, +1, 0, 0];
	var y_pos = [0, 0, -1, +1];
	for (var i = 0; i < 4; ++i)
	{
		var block = FindObject(Find_AtPoint(x_pos[i] * build_grid_x, y_pos[i] * build_grid_y), Find_Exclude(this), Find_Property("is_constructed"), Find_Property(TileKindPropertyName));
		if (!block) continue;
		if (ignore_cycles && block.already_found) continue;
		blocks[1 + x_pos[i]][1 + y_pos[i]] = block;
	}
	last_neighbours_as_matrix = blocks;
	return blocks;
}

private func GetNeighbours(bool ignore_cycles)
{
	var blocks = [];
	for (var neighbour in [neighbours.left, neighbours.right, neighbours.up, neighbours.bottom])
	{
		if (!neighbour || (ignore_cycles && neighbour.already_found)) continue;
		PushBack(blocks, neighbour);
	}
	return blocks;
}

private func IsNeighbour(object other)
{
	return GetIndexOf([neighbours.left, neighbours.right, neighbours.up, neighbours.bottom], other) != -1;
}

private func UpdateGraphics(array neighbours)
{
	neighbours = neighbours || GetNeighboursAsMatrix();
	
	var count = 0;
	for (var axis in neighbours)
		for (var obj in axis)
			if (obj) ++count;
	if (count >= 3 || count == 1)
	{
		SetGraphics("");
	}
	else
	if (count == 2)
	{
		var horizontal = (neighbours[0][1] && neighbours[2][1]);
		var vertical = (neighbours[1][0] && neighbours[1][2]);
		if (vertical)
		{
			SetGraphics("Straight");
			SetR(0);
		}
		else if (horizontal)
		{
			SetGraphics("Straight");
			SetR(90);
		}
		else
		{
			SetGraphics("Edge");
			if (neighbours[1][0] && neighbours[2][1])
				SetR(0);
			else if (neighbours[2][1] && neighbours[1][2])
				SetR(90);
			else if (neighbours[1][2] && neighbours[0][1])
				SetR(180);
			else
				SetR(270);
		}
	}
}