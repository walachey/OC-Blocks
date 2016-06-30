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
	
	for (var o in FindObjects(Find_Func("IsBuildingTile"), Find_Property("is_constructed"), Find_AtPoint(clonk->GetX() + iX - GetX(), clonk->GetY() + iY - GetY())))
	{
		o->SetClrModulation(color);
	}

	return true;
}

public func RejectCollect(id def, object obj)
{
	if (!obj->~IsBucketMaterial()) return true;
	// Can only contain one stackable object.
	if (Contents() && Contents(0)->~IsStackable()) return true;
	return false;
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
