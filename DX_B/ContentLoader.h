#pragma once

#include "ContentWindow.h"

class ContentLoader
{
	public:
		static void BuildFunctionCalls();
		static void LoadContentStage(int index);

		static void Update();

		static void PresentWindow(int index);
		static void PresentOverlay(int indeX);
		static void FullyCloseAndResetWindows();

		static ContentWindow& GetCurrentWindow();
		static ContentWindow& GetCurrentOverlay();

		static ID3D11ShaderResourceView** GetTextureAddress(int index);

	public:
		static uint32_t
			current_interface_id,
			current_overlay_id,
			static_interface_buffer_size,
			static_mesh_buffer_size,
			static_overlay_buffer_size,

			/*
			*	0x1(1)	=	Allow 3D Processing.
			*	0x2(2)	=	Allow Background Texture To Draw.
			*/
			RENDER_BITMASK;

	public:
		static Microsoft::WRL::ComPtr<ID3D11Buffer>
			static_interfaces_buffer,
			static_overlay_buffer,
			static_mesh_buffer;
};
