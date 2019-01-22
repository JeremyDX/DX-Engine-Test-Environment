#pragma once

class Engine
{

	public:
		static Microsoft::WRL::ComPtr<ID3D11Device1> device;
		static Microsoft::WRL::ComPtr<ID3D11DeviceContext1> context;

	public:
		static void Stop();
		static int Boot(int preferred_height, int preferred_width);
};