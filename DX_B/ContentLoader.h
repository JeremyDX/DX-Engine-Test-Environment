#pragma once

#include "ContentWindow.h"
#include "FontResource.h"

class ContentLoader
{
public:
	static void PresentWindow(int index);
	static void PresentOverlay(int index);
	static void LoadContentStage(int index);
	static ContentWindow GetCurrentWindow();
	static TextureResource GetTextureResource(int index);
	static auto GetTextureAddress(int index);
	static ComPtr<ID3D11ShaderResourceView> GetTextureComResource(int index);
	static void UpdateDynamicStringBuffer(const char* text, int length, int font_id);

public:
	static __int32 m_index, s_index, dynamic_interface_buffer_size;

private:
	static void LoadHeaderInformation(int stage);
	static void CheckContentHeaderSize(int size, int index);

	static ContentWindow* interfaces;
	static FontResource* fonts;

public:
	static TextureResource *texture_resources;

private:

	//Current Position Loaded Up To.
	static unsigned __int8 
		loaded_interfaces, 
		loaded_fonts, 
		loaded_textures, 
		loaded_pshaders,
		loaded_vshaders;

	//Highest Position Stored In Memory.
	static unsigned __int16
		max_interfaces, 
		max_fonts, 
		max_textures, 
		max_vshaders,
		max_pshaders;

public:
	static ComPtr<ID3D11Buffer> 
		static_interfaces_buffer,
		static_mesh_buffer,
		dynamic_interfaces_buffer,
		dynamic_mesh_buffer;
};
