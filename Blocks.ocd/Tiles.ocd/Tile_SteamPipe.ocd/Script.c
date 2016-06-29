/**
	Tile_SteamPipe
	

	@author 
*/

#include Lib_BuildingTile

local Name = "$Name$";
local Description = "$Description$";
local Collectible = 1;
local Plane = 14;

local tile_mode = TILE_MODE_LINE;

local IsSteamPipeBuildingTile = true;

local neighbours;

local steam_level = 0;

local open_offset_x;
local open_offset_y;

local steam_particles;

public func Definition()
{
	this.steam_particles = new Particles_Smoke()
	{
		ForceX = PV_Random(-5, 5, 10),
		ForceY = PV_Random(-5, 5, 10),
		R = 255, G = 255, B = 255, 
		Alpha = PV_Linear(16, 0),
		Rotation = PV_Random(360)
	};
}

public func Construction()
{
	neighbours = {left = nil, right = nil, up = nil, bottom = nil};
	this.UpdateGraphics = Tile_Conveyor.UpdateGraphics;
	return inherited();
}

protected func Hit(x, y)
{
	StonyObjectHit(x,y);
	return true;
}

func BuildingCondition()
{
	if (FindObject(Find_AtPoint(), Find_Property("is_constructed"), Find_Not(Find_Or(Find_Func("IsWallBuildingTile"), Find_Func("IsPillarBuildingTile"))), Find_Func("IsBuildingTile"), Find_Exclude(this)))
		return false;

	if (VerticesStuckSemi() == GetVertexNum()+1)
		return false;
	
	if (FindObject(Find_Or(Find_OnLine(-tile_size_x/2-1, 0, tile_size_x/2+1, 0), Find_OnLine(0, -tile_size_y/2-1, 0, tile_size_y/2+2)),
		Find_Exclude(this), Find_Property("is_constructed"), Find_Property("IsSteamPipeBuildingTile")))
		return true;
	
	if (FindObject(Find_AtPoint(), Find_Property("is_constructed"), Find_Func("IsPillarBuildingTile"), Find_Exclude(this)))
		return true;
	
	return false;
}

func Constructed()
{
	_inherited();
	
	var current_neighbours = GetNeighboursAsMatrix();
	AddNeighbour("left", current_neighbours[0][1]);
	AddNeighbour("right", current_neighbours[2][1]);
	AddNeighbour("up", current_neighbours[1][0]);
	AddNeighbour("bottom", current_neighbours[1][2]);
	this->UpdateGraphics(current_neighbours);
	
	var neighbour_count = 0;
	for (var neighbour in [neighbours.left, neighbours.right, neighbours.up, neighbours.bottom])
	{
		if (!neighbour) continue;
		neighbour->UpdateGraphics(neighbour->GetNeighboursAsMatrix());
		++neighbour_count;
	}
	
	if (neighbour_count != 1)
	{
		open_offset_x = nil;
		open_offset_y = nil;
	}
	
	AddTimer("CheckSteam", 10 + Random(3));
	AddTimer("CheckBurningObjects", 20 + Random(10));
}

public func CheckOpenOffset(bool full_check)
{
	var neighbour_list = [neighbours.left, neighbours.right, neighbours.up, neighbours.bottom];
	var directions = ["left", "right", "up", "bottom"];

	open_offset_x = 0;
	open_offset_y = 0;

	var neighbour_count = 0;
	for (var i = 0; i < 4; ++i)
	{
		var neighbour = neighbour_list[i];
		if (!neighbour) continue;
		var direction = directions[i];
		++neighbour_count;
		
		if (direction == "left")
			open_offset_x = build_grid_x/2;
		else if (direction == "right")
			open_offset_x = -build_grid_x/2;
		else if (direction == "up")
			open_offset_y = +build_grid_y/2;
		else if (direction == "bottom")
			open_offset_y = -build_grid_y/2;
	}
	
	if (neighbour_count != 1)
	{
		open_offset_x = nil;
		open_offset_y = nil;
	}
}

public func Destruct()
{
	RemoveTimer("CheckSteam");
	RemoveTimer("CheckBurningObjects");
	OnDetached();
	return inherited(...);
}

public func Destroy()
{
	OnDetached();
	RemoveObject();
}

private func OnDetached()
{
	is_constructed = false;
	
	var mirrored_directions = ["right", "left", "bottom", "up"];
	var neighbour_list = [neighbours.left, neighbours.right, neighbours.up, neighbours.bottom];
	for (var i = 0; i < 4; ++i)
	{
		var neighbour = neighbour_list[i];
		if (!neighbour) continue;
		neighbour->AddNeighbour(mirrored_directions[i], nil);
		neighbour->CheckOpenOffset(true);
		neighbour->UpdateGraphics(neighbour->GetNeighboursAsMatrix());
	}
}

private func CheckBurningObjects()
{
	var total_fire_value = 0;
	for (var obj in FindObjects(Find_AtRect(-build_grid_x, -build_grid_y, 2 * build_grid_x, 5 * build_grid_y), Find_Func("OnFire")))
	{
		var fire_value = obj->GetMass() + obj->~GetFuelAmount() / 10;
		total_fire_value += fire_value;
	}
	DoSteam(total_fire_value);
}

private func CheckSteam()
{
	if (steam_level <= 0) return;
	
	var min_steam_neighbour = nil;
	var neighbour_count = 0;
	for (var neighbour in [neighbours.left, neighbours.right, neighbours.up, neighbours.bottom])
	{
		if (!neighbour) continue;
		++neighbour_count;
		if (!min_steam_neighbour || neighbour.steam_level < min_steam_neighbour.steam_level)
			min_steam_neighbour = neighbour;
	}
	
	var change = 0;
	if (min_steam_neighbour)
		change = (steam_level - min_steam_neighbour.steam_level) / 2;
		
	if (!(change > 0))
	{
		if (open_offset_x != nil)
		{
			var loss = Max(5, steam_level / 4);
			CreateParticle("Dust", PV_Random(open_offset_x - 5, open_offset_x + 5), PV_Random(open_offset_y - 5, open_offset_y + 5), 0, 0, PV_Random(10, 200), this.steam_particles, 10 * loss);
			DoSteam(-loss);
			if (!Random(3))
				Sound("Liquids::Sizzle", {pitch = 5 + Random(20), volume = 20});
		}
		else
		{
			var rnd = Random(steam_level);
			var limit = 100 * (5 - neighbour_count);
			if (rnd > limit)
			{
				CreateParticle("Dust", PV_Random(-4, 4), PV_Random(-4, 4), PV_Random(-10, 10), PV_Random(-10, 10), PV_Random(5, 20), this.steam_particles, 5 * steam_level);
				Destroy();
			}
			else
			if (rnd > limit / 2)
			{
				if (!Random(3))
					SoundAt("Liquids::Boiling");
				CreateParticle("Dust", PV_Random(-4, 4), PV_Random(-4, 4), PV_Random(-10, 10), PV_Random(-10, 10), PV_Random(5, 20), this.steam_particles, 20);
			}
			
		}
		return;
	}
	
	min_steam_neighbour->DoSteam(change);
	DoSteam(-change);
}

public func DoSteam(int change)
{
	steam_level = Max(0, steam_level + change);
	var discolored = 255 - Min(steam_level, 200);
	SetClrModulation(RGB(255, discolored, discolored));
}

private func AddNeighbour(string direction, object neighbour, bool no_recursion)
{
	neighbours[direction] = neighbour;
	if (!neighbour) return;

	if (!no_recursion)
	{
		var mapping = {left = "right", right = "left", up = "bottom", bottom = "up"};
		neighbour->AddNeighbour(mapping[direction], this, true);
		neighbour->CheckOpenOffset();
	}

	open_offset_x = 0;
	open_offset_y = 0;
	if (direction == "left")
		open_offset_x = build_grid_x/2;
	else if (direction == "right")
		open_offset_x = -build_grid_x/2;
	else if (direction == "up")
		open_offset_y = +build_grid_y/2;
	else if (direction == "bottom")
		open_offset_y = -build_grid_y/2;
}

private func GetNeighboursAsMatrix(bool ignore_cycles)
{
	var blocks = [[nil, nil, nil], [nil, nil, nil], [nil, nil, nil]];
	var x_pos = [-1, +1, 0, 0];
	var y_pos = [0, 0, -1, +1];
	for (var i = 0; i < 4; ++i)
	{
		var block = FindObject(Find_AtPoint(x_pos[i] * build_grid_x, y_pos[i] * build_grid_y), Find_Property("is_constructed"), Find_Property("IsSteamPipeBuildingTile"));
		if (!block) continue;
		if (ignore_cycles && block.already_found) continue;
		blocks[1 + x_pos[i]][1 + y_pos[i]] = block;
	}
	return blocks;
}

private func GetNeighbours(bool ignore_cycles)
{
	var blocks = [];
	var x_pos = [-1, +1, 0, 0];
	var y_pos = [0, 0, -1, +1];
	for (var i = 0; i < 4; ++i)
	{
		var block = FindObject(Find_AtPoint(x_pos[i] * build_grid_x, y_pos[i] * build_grid_y), Find_Property("is_constructed"), Find_Property("IsSteamPipeBuildingTile"));
		if (!block) continue;
		if (block.already_found && ignore_cycles) continue;
		PushBack(blocks, block);
	}
	return blocks;
}

private func IsNeighbour(object other)
{
	return other->GetID() == GetID() && ObjectDistance(other) <= Distance(0, 0, build_grid_x, build_grid_y);
}


local Components = {Wood = 1};
public func IsToolProduct() { return true; }
