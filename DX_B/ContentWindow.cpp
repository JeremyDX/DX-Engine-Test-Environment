#include "pch.h"

#include "ContentWindow.h"

#include "XGameInput.h"
#include "GameTime.h"
#include "ContentLoader.h"

ContentWindow::ContentWindow() { }

ContentWindow::~ContentWindow() { }

/*
* This Function Checks if Any Button Was Pressed or 10 Seconds Passed.
* @Objective -> we proceed to the next window index.
* @Warning -> No Security checks if a window is actually available.
*/
void UpdateToWindow(void)
{
	if (XGameInput::AnyButtonsPressed() || GameTime::ElapsedWindowFrameTicks() >= 600)
	{
		ContentLoader::PresentWindow(ContentLoader::m_index + 1);
	}
}

void DoNothing(void) { }

void ContentWindow::SetUpdateProc(int index)
{
	if (index == 1)
	{
		update = UpdateToWindow;
	} else {
		update = DoNothing;
	}
}

