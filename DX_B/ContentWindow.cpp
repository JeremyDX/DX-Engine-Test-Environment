#include "pch.h"
#include "ContentWindow.h"

unsigned __int32 ContentWindow::ticks = 0;

ContentWindow::ContentWindow() { }

ContentWindow::~ContentWindow() { }

void UpdateToWindow(void)
{
	++ContentWindow::ticks;
 	if ((ContentWindow::ticks >= 60 && XGameInput::GetButtonBitSet() > 0) || ContentWindow::ticks >= 600)
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

