/**
	Bucket
	Transport earth from one spot to another to form the landscape.
	Replaces the old earth chunks in their behaviour.

	@author Clonkonaut
*/


// Maximum distance at which material is collected / spilled
local maxreach = 15;

local color;

public func GetCarryMode() { return CARRY_HandBack; }
public func GetCarryBone() { return "main"; }
public func GetCarryTransform()
{
	return Trans_Mul(Trans_Rotate(-90, 0, 1, 0), Trans_Translate(3500, 0, -4000));
}

public func RejectUse(object clonk)
{
	return !clonk->HasHandAction(false, false, true);
}

public func SetColor(clr)
{
	color = clr;
	SetClrModulation(clr);
}

public func ControlUse(object clonk, int iX, int iY)
{
	if(!color)
		return;
	
	var p = 0;
	var obj = nil;
	
	for (var o in FindObjects(Find_AtPoint(clonk->GetX() + iX - GetX(), clonk->GetY() + iY - GetY()), 
	Find_Or(Find_Func("IsSolidBuildingTile"), Find_Func("IsWallBuildingTile")), Find_Property("can_be_colored"), Find_Property("is_constructed")))
	{
		if (o.Plane > p)
		{
			obj = o;
			p = o.Plane;
		}
	}
	
	if (obj)
		obj->SetClrModulation(color);

	return true;
}



protected func Hit()
{
	Sound("Hits::BucketHit?");
}

// Can collect IsBucketMaterial?
public func IsBucket() { return true; }
public func IsTool() { return true; }
public func IsToolProduct() { return true; }


protected func Definition(def)
{
	SetProperty("PictureTransformation", Trans_Mul(Trans_Translate(500,400,0), Trans_Rotate(-10,1,0,0), Trans_Rotate(30,0,1,0), Trans_Rotate(+25,0,0,1), Trans_Scale(1100)),def);
}

local Name = "$Name$";
local Description = "$Description$";
local Collectible = true;
local ForceFreeHands = true;
local Components = {Wood = 1, Metal = 1};
