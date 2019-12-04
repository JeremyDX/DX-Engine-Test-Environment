#pragma once

#include <cstdint>

//80 Bytes Per Window / Overlay Componenet Per Content Package.
struct ContentWindow
{
	uint8_t texture_count;

	uint16_t vertex_offsets[4];
	uint8_t texture_ids[4];

	void(*update)();
};