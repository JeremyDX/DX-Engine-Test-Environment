#pragma once

class ContentOverlay
{
	public:
		void(*update)();
		uint16_t texture_index[2];
		uint16_t offsets[2];
		uint8_t total_textures;

	public:
		void SetUpdateProc(int index);
};
