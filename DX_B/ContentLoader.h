#pragma once

#include "ContentWindow.h"
#include "ContentOverlay.h"
#include "TextureResource.h"
#include "FontResource.h"

class ContentLoader
{
	public:
		static void PresentWindow(int index);
		static void ClearWindow();
		static void PresentOverlay(int index);
		static void LoadContentStage(int index);
		static void AllocateVertexBuffers();
		static ContentWindow & GetCurrentWindow();
		static ContentOverlay & GetCurrentOverlay();

		static ID3D11ShaderResourceView** GetTextureAddress(int index);

		static void SwapQuadsPosition(int offset_a, int offset_b);
		static void UpdateOverlayString(int begin, const char * text, int zeroing_size);
		static void SendUpdatedBufferToGpu();

	public:
		static __int32 
			m_index,
			s_index,
			static_interface_buffer_size,
			static_mesh_buffer_size,
			static_overlay_buffer_size;

		static bool ALLOW_3D_PROCESSING;

	private:
		static void LoadHeaderInformation(int stage);

		static void LoadMenuStage();
		static void LoadWorldStage();

		static ContentWindow* interfaces;
		static ContentOverlay* overlays;
		static FontResource* fonts;
		static ID3D11ShaderResourceView** texture_resources;

	private:

		static unsigned __int16
			max_overlays,
			max_interfaces, 
			max_fonts, 
			max_textures, 
			max_vshaders,
			max_pshaders;

	public:
		static Microsoft::WRL::ComPtr<ID3D11Buffer>
			static_interfaces_buffer,
			static_overlay_buffer,
			static_mesh_buffer;
};
