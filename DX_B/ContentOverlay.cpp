#include "pch.h"
#include "ContentLoader.h"
#include "ContentOverlay.h"
#include "CameraEngine.h"
#include "GameTime.h"

void DrawFps() 
{ 

}

void DrawCameraDetails() 
{ 
	CameraEngine::CreateDebugOverlay();
}

void ContentOverlay::SetUpdateProc(int index)
{
	if (index == 2)
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
