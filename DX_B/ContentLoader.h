#pragma once

#include "ContentWindow.h"
#include "TextureResource.h"
#include "FontResource.h"

class ContentLoader
{
	public:
		static void PresentWindow(int index);
		static void ClearWindow();
		static void PresentOverlay(int index);
		static void LoadContentStage(int index);
		static ContentWindow& GetCurrentWindow();

		static ID3D11ShaderResourceView** GetTextureAddress(int index);

		static void UpdateDynamicStringBuffer(const char* text, int length, int font_id);
		static void SwapQuadsPosition(int offset_a, int offset_b);

		static void SendUpdatedBufferToGpu();

	public:
		static __int32 m_index, s_index, dynamic_interface_buffer_size;
		static bool ALLOW_3D_PROCESSING;

	private:
		static void LoadHeaderInformation(int stage);

		static void LoadMenuStage();
		static void LoadWorldStage();

		static ContentWindow* interfaces;
		static FontResource* fonts;
		static ID3D11ShaderResourceView** texture_resources;

	private:

		static unsigned __int16
			max_interfaces, 
			max_fonts, 
			max_textures, 
			max_vshaders,
			max_pshaders;

	public:
		static Microsoft::WRL::ComPtr<ID3D11Buffer>
			static_interfaces_buffer,
			static_mesh_buffer,
			dynamic_interfaces_buffer,
			dynamic_mesh_buffer;
};
