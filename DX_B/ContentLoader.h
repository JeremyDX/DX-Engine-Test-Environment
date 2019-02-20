#pragma once

#include "ContentWindow.h"
#include "ContentOverlay.h"
#include "TextureResource.h"
#include "FontResource.h"

class ContentLoader
{
	public:
		static const float CreateShaderColor(const float brightness, const float alpha);
		static void WritePixelsToShaderIndex(uint32_t * data, int width, int height, int index);
		static void PresentWindow(int index);
		static void ClearWindow();
		static void PresentOverlay(int index);
		static void LoadContentStage(int index);
		static void AllocateVertexBuffers();
		static ContentWindow & GetCurrentWindow();
		static ContentOverlay & GetCurrentOverlay();

		static ID3D11ShaderResourceView** GetTextureAddress(int index);

		static void RotateOverlayTexture(int begin, Float2 verts[4]);
		static void UpdateOverlayString(int children_id, const char * text);
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
		static void LoadSimple2DWorld();

	public:
		static Microsoft::WRL::ComPtr<ID3D11Buffer>
			static_interfaces_buffer,
			static_overlay_buffer,
			static_mesh_buffer;
};
