#include "pch.h"
#include "ContentLoader.h"
#include "ContentOverlay.h"
#include "CameraEngine.h"
#include "GameTime.h"
#include "XGameInput.h"

void DrawFps() 
{ 

}

void DrawCameraDetails() 
{ 
	CameraEngine::CreateDebugOverlay();
}

void DrawRulesFor2D()
{

}

void ContentOverlay::SetUpdateProc(int index)
{
	if (index == 3)
	{
		update = DrawRulesFor2D;
	}
	else if (index == 2)
	{
		update = DrawCameraDetails;
	}
	else if (index == 1)
	{
		update = DrawFps;
	}
	else {

	}
}
