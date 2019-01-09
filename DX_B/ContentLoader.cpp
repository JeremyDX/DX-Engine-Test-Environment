#include "pch.h"
#include "ContentLoader.h"

#include "Engine.h"
#include "WICTextureLoader.h"

ContentWindow* ContentLoader::interfaces;
FontResource* ContentLoader::fonts;
TextureResource* ContentLoader::texture_resources;

int ContentLoader::m_index, ContentLoader::s_index, ContentLoader::dynamic_interface_buffer_size;

void ContentLoader::PresentWindow(int index)
{
	GameTime::ResetWindowTimeStamp();
	m_index = index;
}

void ContentLoader::PresentOverlay(int index)
{
	s_index = index;
}

void ContentLoader::LoadContentStage(int stage)
{
	LoadHeaderInformation(stage);
}

ContentWindow ContentLoader::GetCurrentWindow()
{
	return interfaces[m_index];
}

TextureResource& ContentLoader::GetTextureResource(int index)
{
	return texture_resources[index];
}

ID3D11ShaderResourceView** ContentLoader::GetTextureAddress(int index)
{
	return texture_resources[index].m_texture.GetAddressOf();
}

ComPtr<ID3D11ShaderResourceView> ContentLoader::GetTextureComResource(int index)
{
	return texture_resources[index].m_texture;
}

/* 
 * Hardcoded Interfaces For Now :(
 * Don't worry we'll have a content builder soon enough.
 */
void ContentLoader::LoadHeaderInformation(int stage)
{
	interfaces = new ContentWindow[3];
	max_interfaces = 3;

	fonts = new FontResource[4];
	max_fonts = 4;

	texture_resources = new TextureResource[7];
	max_textures = 7;

	Vertex32Byte vertsToDraw[4096];

	vertsToDraw[0] = { -1.0F,  1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	0.0F, 0.0F};
	vertsToDraw[1] = {  1.0F, -1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	1.0F, 1.0F};
	vertsToDraw[2] = { -1.0F, -1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	0.0F, 1.0F};

	vertsToDraw[3] = {  1.0F, -1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	1.0F, 1.0F};
	vertsToDraw[4] = { -1.0F,  1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	0.0F, 0.0F};
	vertsToDraw[5] = {  1.0F,  1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	1.0F, 0.0F};
	
	interfaces[0].background_color = 0;
	interfaces[0].background_shader_id = -1;
	
	CreateWICTextureFromFile(Engine::device.Get(), nullptr, L"Assets/RGB_TransparencyMap.png", nullptr, texture_resources[0].m_texture.GetAddressOf(), 0);
	CreateWICTextureFromFile(Engine::device.Get(), nullptr, L"Assets/0_FONT.png", nullptr, texture_resources[1].m_texture.GetAddressOf(), 0);
	CreateWICTextureFromFile(Engine::device.Get(), nullptr, L"Assets/1_FONT.png", nullptr, texture_resources[2].m_texture.GetAddressOf(), 0);
	CreateWICTextureFromFile(Engine::device.Get(), nullptr, L"Assets/2_FONT.png", nullptr, texture_resources[3].m_texture.GetAddressOf(), 0);
	CreateWICTextureFromFile(Engine::device.Get(), nullptr, L"Assets/3_FONT.png", nullptr, texture_resources[4].m_texture.GetAddressOf(), 0);
	CreateWICTextureFromFile(Engine::device.Get(), nullptr, L"Assets/Image_0.png", nullptr, texture_resources[5].m_texture.GetAddressOf(), 0);
	CreateWICTextureFromFile(Engine::device.Get(), nullptr, L"Assets/Image_1.png", nullptr, texture_resources[6].m_texture.GetAddressOf(), 0);

	fonts[0].CreateGlyphMapping(0);
	fonts[1].CreateGlyphMapping(1);
	fonts[2].CreateGlyphMapping(2);
	fonts[3].CreateGlyphMapping(3);

	fonts[0].font_size = 38;
	fonts[0].texture_width = 413;
	fonts[0].texture_height = 235;

	fonts[1].font_size = 105;
	fonts[1].texture_width = 1185;
	fonts[1].texture_height = 637;

	fonts[2].font_size = 40;
	fonts[2].texture_width = 422;
	fonts[2].texture_height = 247;

	fonts[3].font_size = 31;
	fonts[3].texture_width = 348;
	fonts[3].texture_height = 193;

	Float3 v = {1.0F, 1.0F, 1.0F};

	int begin = 6;
	int offset = fonts[1].AddStringToBuffer(L"Developed By", vertsToDraw, v, 6, 0, 50, 1);
	offset = fonts[1].AddStringToBuffer(L"Jeremy DX", vertsToDraw, v, offset, 0, 300, 1);

	interfaces[0].state_changes = 1;
	interfaces[0].state_change_alias[0] = 2;
	interfaces[0].state_vertex_offsets[0] = begin;
	interfaces[0].state_vertex_sizes[0] = offset - begin;
	interfaces[0].SetUpdateProc(1);

	begin = offset;

	offset = fonts[3].AddStringToBuffer(L"This Engine Is In Testing Phase", vertsToDraw, v, offset, 0, 100, 1);
	offset = fonts[3].AddStringToBuffer(L"\n\nCurrently testing Content Loading", vertsToDraw, v, offset, 0, 100, 1);
	offset = fonts[3].AddStringToBuffer(L"\n\n\n\nYou can press any button to continue.", vertsToDraw, v, offset, 0, 100, 1);

	offset = fonts[3].AddStringToBuffer(L"Project Source: https://github.com/JeremyDX/DX_B ", vertsToDraw, v, offset, 0, 450, 1);
	offset = fonts[3].AddStringToBuffer(L"YouTube Channel: https://www.youtube.com/user/DxXNA/", vertsToDraw, v, offset, 0, 500, 1);

	interfaces[1].state_changes = 1;
	interfaces[1].state_change_alias[0] = 4;
	interfaces[1].state_vertex_offsets[0] = begin;
	interfaces[1].state_vertex_sizes[0] = offset - begin;
	interfaces[1].SetUpdateProc(1);

	begin = offset;

	Float3 Green = { 0.0F, 1.0F, 0.0F };
	offset = fonts[1].AddStringToBuffer(L"Undead Survival", vertsToDraw, v, offset, 0, 10, 1);
	offset = fonts[1].AddStringToBuffer(L"Game Engine", vertsToDraw, v, offset, 0, 130, 1);

	interfaces[2].state_vertex_offsets[0] = begin;
	interfaces[2].state_vertex_sizes[0] = offset - begin;

	begin = offset;

	offset = XModelMesh::CreateTexturedSquare(vertsToDraw, offset, Green, 360, 360, 50, 300);
	offset = XModelMesh::CreateTexturedSquare(vertsToDraw, offset, v, 360, 360, 460, 300);
	offset = XModelMesh::CreateTexturedSquare(vertsToDraw, offset, v, 360, 360, 870, 300);

	interfaces[2].state_vertex_offsets[1] = begin;
	interfaces[2].state_vertex_sizes[1] = offset - begin;
	
	interfaces[2].state_changes = 2;
	interfaces[2].state_change_alias[0] = 2; //Title Text.
	interfaces[2].state_change_alias[1] = 6; //Bottom Images.
	
	interfaces[2].SetUpdateProc(0);

	D3D11_BUFFER_DESC bd = { 0 };
	bd.ByteWidth = sizeof(Vertex32Byte) * offset;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	float *floatVersion = new float[offset * 8];
	int n = -1;
	for (int i = 0; i < offset; ++i)
	{
		floatVersion[++n] = vertsToDraw[i]._X;
		floatVersion[++n] = vertsToDraw[i]._Y;
		floatVersion[++n] = vertsToDraw[i]._Z;

		floatVersion[++n] = vertsToDraw[i]._1;	
		floatVersion[++n] = vertsToDraw[i]._2;
		floatVersion[++n] = vertsToDraw[i]._3;
		
		floatVersion[++n] = vertsToDraw[i]._U;
		floatVersion[++n] = vertsToDraw[i]._V;
	}

	D3D11_SUBRESOURCE_DATA srd = { floatVersion, 0, 0 };
	Engine::device->CreateBuffer(&bd, &srd, static_interfaces_buffer.GetAddressOf());

	
}

void ContentLoader::CheckContentHeaderSize(int size, int index)
{

}

bool created = false;

void ContentLoader::UpdateDynamicStringBuffer(const char* text, int length, int font_id)
{
	Vertex32Byte vertsToDraw[12000];

	int begin = 0;
	Float3 v;
	v._1 = 1.0f;
	v._2 = 1.0f;
	v._3 = 1.0f;

	int offset = fonts[font_id].AddStringToBuffer(text, vertsToDraw, v, 0, 10, 10, 0);

	dynamic_interface_buffer_size = offset;

	if (created)
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		Engine::context->Map(dynamic_interfaces_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource); 
		
		memcpy(resource.pData, &vertsToDraw, sizeof(Vertex32Byte) * offset);
		
		Engine::context->Unmap(dynamic_interfaces_buffer.Get(), 0);
	}
	else 
	{
		D3D11_BUFFER_DESC bd = { 0 };
		bd.ByteWidth = sizeof(Vertex32Byte) * 12000;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA srd = { vertsToDraw, 0, 0 };
		Engine::device->CreateBuffer(&bd, &srd, dynamic_interfaces_buffer.GetAddressOf());
		created = true;
	}
}

unsigned __int8 
	ContentLoader::loaded_interfaces = 0, 
	ContentLoader::loaded_fonts = 0, 
	ContentLoader::loaded_textures = 0,
	ContentLoader::loaded_vshaders = 0,
	ContentLoader::loaded_pshaders = 0;

unsigned __int16
	ContentLoader::max_interfaces = 0, 
	ContentLoader::max_fonts = 0, 
	ContentLoader::max_textures = 0,  
	ContentLoader::max_vshaders = 0,
	ContentLoader::max_pshaders = 0;

ComPtr<ID3D11Buffer>
	ContentLoader::static_interfaces_buffer,
	ContentLoader::static_mesh_buffer, 
	ContentLoader::dynamic_interfaces_buffer,  
	ContentLoader::dynamic_mesh_buffer;