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
	if (XGameInput::AnyButtonPressed() || GameTime::ElapsedWindowFrameTicks() >= 600)
	{
		ContentLoader::PresentWindow(ContentLoader::m_index + 1);
	}
}

void ProcessMenuButtons(void)
{
	ContentWindow& cw = ContentLoader::GetCurrentWindow();
	cw.children();
	if (XGameInput::AnyOfTheseButtonsArePressed(
		XBOX_CONTROLLER::A_BUTTON | XBOX_CONTROLLER::X_BUTTON | 
		XBOX_CONTROLLER::MENU_BUTTON | XBOX_CONTROLLER::VIEW_BUTTON
	))
	{
		switch (cw.menu_index)
		{
			case 0:
				//ContentLoader::ClearWindow();
				//ContentLoader::LoadContentStage(2);
				break;

			case 1:
				ContentLoader::ClearWindow();
				ContentLoader::LoadContentStage(1);
				break;

			case 2:
				ContentLoader::ClearWindow();
				ContentLoader::LoadContentStage(0);
				ContentLoader::PresentWindow(0);
				break;
		}
	}
}

void MenuUpdateDownUp(void)
{
	ContentWindow cw = ContentLoader::GetCurrentWindow();
}

void MenuUpdateLeftRight(void)
{
	ContentWindow & cw = ContentLoader::GetCurrentWindow();
	if (XGameInput::AllOfTheseButtonsArePressed
	(
		XBOX_CONTROLLER::D_PAD_LEFT
	))
	{
		int oldPosition = cw.state_vertex_offsets[1] + cw.menu_index * 6;
		--cw.menu_index;
		if (cw.menu_index < 0)
			cw.menu_index = cw.menu_size - 1;
		int newPosition = cw.state_vertex_offsets[1] + cw.menu_index * 6;
		ContentLoader::SwapQuadsPosition(oldPosition, newPosition);
	}
	else if (XGameInput::AllOfTheseButtonsArePressed
	(
		XBOX_CONTROLLER::D_PAD_RIGHT
	))
	{
		int oldPosition = cw.state_vertex_offsets[1] + cw.menu_index * 6;
		++cw.menu_index;
		if (cw.menu_index >= cw.menu_size)
			cw.menu_index = 0;
		int newPosition = cw.state_vertex_offsets[1] + cw.menu_index * 6;
		ContentLoader::SwapQuadsPosition(oldPosition, newPosition);
	}
}

void Play2DWorld(void) 
{
	ContentWindow & cw = ContentLoader::GetCurrentWindow();
}

void DoNothing(void) { }

void ContentWindow::SetUpdateProc(int index)
{
	if (index == 3)
	{
		update = Play2DWorld;
	}
	else if (index == 2)
	{
		update = ProcessMenuButtons;
	}
	else if (index == 1)
	{
		update = UpdateToWindow;
	} 
	else 
	{
		update = DoNothing;
	}
}

void ContentWindow::SetChildUpdateProc(int index, int size, int disabled_bits)
{
	this->disabled_menu_bits = disabled_bits;
	this->menu_size = size;
	this->menu_index = 0;
	if (index == 0)
		children = MenuUpdateDownUp;
	else if (index == 1)
		children = MenuUpdateLeftRight;
	else
		children = DoNothing;
}

