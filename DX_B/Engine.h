#pragma once

class Engine
{
	public:
		static void Stop();
		static  int Boot(int preferred_height, int preferred_width);

	public:
		static Microsoft::WRL::ComPtr<ID3D11Device1> device;
		static Microsoft::WRL::ComPtr<ID3D11DeviceContext1> context;

		static uint16_t PREFERRED_CANVAS_WIDTH, PREFERRED_CANVAS_HEIGHT;
};