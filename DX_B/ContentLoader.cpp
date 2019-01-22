#include "pch.h"
#include "ContentLoader.h"

#include "Engine.h"
#include "GameTime.h"
#include "XModelMesh.h"
#include "XWicLoader.h"

using namespace DirectX;

ContentWindow* ContentLoader::interfaces = NULL;
FontResource* ContentLoader::fonts = NULL;
ID3D11ShaderResourceView** ContentLoader::texture_resources = NULL;

int ContentLoader::m_index = -1, 
	ContentLoader::s_index = -1, 
	ContentLoader::dynamic_interface_buffer_size = -1;

bool ContentLoader::ALLOW_3D_PROCESSING = false;

Vertex32Byte vertsToDraw[4096];

void ContentLoader::PresentWindow(int index)
{
	m_index = index;

	GameTime::ResetWindowTimeStamp();
}

void ContentLoader::ClearWindow()
{
	m_index = -1;
	GameTime::ResetWindowTimeStamp();
}

void ContentLoader::PresentOverlay(int index)
{
	s_index = index;
}

void ContentLoader::LoadContentStage(int stage)
{
	LoadHeaderInformation(stage);
}

ContentWindow& ContentLoader::GetCurrentWindow()
{
	return interfaces[m_index];
}


ID3D11ShaderResourceView** ContentLoader::GetTextureAddress(int index)
{
	return &texture_resources[index];
}

/* 
 * Hardcoded Interfaces For Now :(
 * Don't worry we'll have a content builder soon enough.
 */

bool test1 = true;
bool test2 = true;

void ContentLoader::LoadHeaderInformation(int stage)
{
	if (stage == 0)
		LoadMenuStage();
	else if (stage == 1)
		LoadWorldStage();
}

void ContentLoader::LoadWorldStage()
{
	if (max_interfaces != 0)
		delete[] interfaces;

	max_interfaces = 0;

	if (max_fonts != 0)
		delete[] fonts;

	max_fonts = 0;

	if (max_textures != 0)
	{
		for (int i = 0; i < max_textures; ++i)
			texture_resources[i]->Release();
	}

	texture_resources = new ID3D11ShaderResourceView*[2];
	max_textures = 2;

	Microsoft::WRL::ComPtr<ID3D11Resource> resource;

	CreateWICTextureFromFile(Engine::device.Get(), Engine::context.Get(), L"Assets/SMILEY512.png", resource.GetAddressOf(), &texture_resources[0], 0);
	CreateWICTextureFromFile(Engine::device.Get(), Engine::context.Get(), L"Assets/RGB_TransparencyMap.png", resource.GetAddressOf(), &texture_resources[1], 0);
	
	Engine::context->GenerateMips(texture_resources[0]);
	Engine::context->GenerateMips(texture_resources[1]);

	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
	resource.As(&tex);

	D3D11_TEXTURE2D_DESC desc;
	tex->GetDesc(&desc);

	Float3 Color = { 1.0f, 1.0f, 1.0f };

	float
		xPos = 0.0f,
		yPos = 0.0f,
		zPos = 0.0f,
		SIZE = 32.0f * 8,
		SIZE_2 = 16.0f * 8;
	
	vertsToDraw[0] = { xPos + -SIZE, yPos , zPos + -SIZE, Color._1, Color._2, Color._3,	0.0f, 0.0f };
	vertsToDraw[1] = { xPos + -SIZE, yPos , zPos +  SIZE, Color._1, Color._2, Color._3,   0.0f, SIZE_2 };
	vertsToDraw[2] = { xPos +  SIZE, yPos , zPos + -SIZE, Color._1, Color._2, Color._3,   SIZE_2, 0.0f };

	vertsToDraw[3] = { xPos +  SIZE, yPos , zPos + -SIZE, Color._1, Color._2, Color._3,   SIZE_2, 0.0f };
	vertsToDraw[4] = { xPos + -SIZE, yPos , zPos +  SIZE, Color._1, Color._2, Color._3,   0.0f,SIZE_2 };
	vertsToDraw[5] = { xPos +  SIZE, yPos , zPos +  SIZE, Color._1, Color._2, Color._3,	SIZE_2, SIZE_2 };

	int offset = 6;

	if (test2)
	{
		test2 = false;
		D3D11_BUFFER_DESC bd = { 0 };
		bd.ByteWidth = sizeof(Vertex32Byte) * 4096;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA srd = { vertsToDraw, 0, 0 };
		Engine::device->CreateBuffer(&bd, &srd, static_mesh_buffer.GetAddressOf());
	}
	else
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		Engine::context->Map(static_mesh_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, &vertsToDraw, sizeof(Vertex32Byte) * offset);
		Engine::context->Unmap(static_mesh_buffer.Get(), 0);
	}

	ALLOW_3D_PROCESSING = true;

}

void ContentLoader::LoadMenuStage()
{
	if (max_interfaces != 0)
		delete[] interfaces;

	if (max_fonts != 0)
		delete[] fonts;

	if (max_textures != 0)
	{
		for (int i = 0; i < max_textures; ++i)
			texture_resources[i]->Release();
	}

	interfaces = new ContentWindow[3];
	max_interfaces = 3;

	fonts = new FontResource[4];
	max_fonts = 4;

	texture_resources = new ID3D11ShaderResourceView*[7];
	max_textures = 7;

	vertsToDraw[0] = { -1.0F,  1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	0.0F, 0.0F};
	vertsToDraw[1] = {  1.0F, -1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	1.0F, 1.0F};
	vertsToDraw[2] = { -1.0F, -1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	0.0F, 1.0F};

	vertsToDraw[3] = {  1.0F, -1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	1.0F, 1.0F};
	vertsToDraw[4] = { -1.0F,  1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	0.0F, 0.0F};
	vertsToDraw[5] = {  1.0F,  1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	1.0F, 0.0F};
	
	interfaces[0].background_color = 0;
	interfaces[0].background_shader_id = -1;
	
	static const wchar_t* PATHS[7] = 
	{
			L"Assets/RGB_TransparencyMap.png",
			L"Assets/0_FONT.png",
			L"Assets/1_FONT.png",
			L"Assets/2_FONT.png",
			L"Assets/3_FONT.png",
			L"Assets/Image_0.png",
			L"Assets/Image_1.png"
	};

	for (int n = 0; n < 7; ++n)
	{
		CreateWICTextureFromFile(Engine::device.Get(), nullptr, PATHS[n], nullptr, &texture_resources[n], 0);
	}

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
	
	interfaces[2].SetUpdateProc(2); //Contains Menu's.
	interfaces[2].SetChildUpdateProc(1, 3, 0); //Left/Right Menu Size 3. No Disabled Bits.

	if (test1)
	{
		test1 = false;
		D3D11_BUFFER_DESC bd = { 0 };
		bd.ByteWidth = sizeof(Vertex32Byte) * 4096;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		bd.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA srd = { vertsToDraw, 0, 0 };
		Engine::device->CreateBuffer(&bd, &srd, static_interfaces_buffer.GetAddressOf());
	}
	else 
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		Engine::context->Map(static_interfaces_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, &vertsToDraw, sizeof(Vertex32Byte) * offset);
		Engine::context->Unmap(static_interfaces_buffer.Get(), 0);
	}
}

void ContentLoader::SwapQuadsPosition(int offset_a, int offset_b)
{
	Float3 W = { 1.0f, 1.0f, 1.0f };
	Float3 G = { 0.0f, 1.0f, 0.0f };

	for (int a = 0; a < 3; ++a)
	{
		vertsToDraw[offset_a]._1 = W._1;
		vertsToDraw[offset_a]._2 = W._2;
		vertsToDraw[offset_a]._3 = W._3;
		++offset_a;
		vertsToDraw[offset_a]._1 = W._1;
		vertsToDraw[offset_a]._2 = W._2;
		vertsToDraw[offset_a]._3 = W._3;
		++offset_a;
	}

	for (int a = 0; a < 3; ++a)
	{
		vertsToDraw[offset_b]._1 = G._1;
		vertsToDraw[offset_b]._2 = G._2;
		vertsToDraw[offset_b]._3 = G._3;
		++offset_b;
		vertsToDraw[offset_b]._1 = G._1;
		vertsToDraw[offset_b]._2 = G._2;
		vertsToDraw[offset_b]._3 = G._3;
		++offset_b;
	}
}

void ContentLoader::SendUpdatedBufferToGpu()
{
	D3D11_MAPPED_SUBRESOURCE resource;
	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	Engine::context->Map(static_interfaces_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	memcpy(resource.pData, vertsToDraw, 4096 * 32);

	Engine::context->Unmap(static_interfaces_buffer.Get(), 0);
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

unsigned __int16
	ContentLoader::max_interfaces = 0, 
	ContentLoader::max_fonts = 0, 
	ContentLoader::max_textures = 0,  
	ContentLoader::max_vshaders = 0,
	ContentLoader::max_pshaders = 0;

Microsoft::WRL::ComPtr<ID3D11Buffer>
	ContentLoader::static_interfaces_buffer,
	ContentLoader::static_mesh_buffer, 
	ContentLoader::dynamic_interfaces_buffer,  
	ContentLoader::dynamic_mesh_buffer;