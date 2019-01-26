#pragma once

class ContentOverlay
{
	public:
		void(*update)();

	public:
		void SetUpdateProc(int index);
};
