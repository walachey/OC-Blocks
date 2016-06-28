
global func FxPreviewTimer()
{
	for (var o in FindObjects(Find_Func("IsPreview")))
	{
		if(o->PreviewBuildingCondition())
			o->SetClrModulation(RGBa(255, 255, 255, 128));
		else
			o->SetClrModulation(RGBa(255, 0, 0, 128));
	}
}