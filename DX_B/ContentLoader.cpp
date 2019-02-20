#include "pch.h"
#include "ContentLoader.h"
#include "ScreenManager.h"

#include "Engine.h"
#include "GameTime.h"
#include "XModelMesh.h"
#include "XWicLoader.h"
#include "XGameInput.h"

using namespace DirectX;

static uint16_t max_overlays = 0;
static uint16_t max_interfaces = 0;
static uint16_t max_fonts = 0;
static uint16_t max_textures = 0;
static uint16_t max_vshaders = 0;
static uint16_t max_pshaders = 0;

static ContentWindow* interfaces = NULL;
static ContentOverlay* overlays = NULL;
static FontResource* fonts = NULL;
static ID3D11ShaderResourceView** texture_resources = NULL;

int32_t ContentLoader::m_index = -1;
int32_t ContentLoader::s_index = -1;

int32_t ContentLoader::static_interface_buffer_size = -1;
int32_t ContentLoader::static_mesh_buffer_size = -1;
int32_t ContentLoader::static_overlay_buffer_size = -1;

Microsoft::WRL::ComPtr<ID3D11Buffer>
ContentLoader::static_interfaces_buffer,
ContentLoader::static_mesh_buffer,
ContentLoader::static_overlay_buffer;

bool ContentLoader::ALLOW_3D_PROCESSING = false;

Vertex32Byte MeshVerts[32768];
Vertex32Byte InterfaceVerts[4096];
Vertex32Byte OverlayVerts[2000];

const float ContentLoader::CreateShaderColor(const float brightness, const float alpha)
{
	const int a = (int)(brightness * 255);
	const int b = (int)(alpha * 255);
	return (float)((a << 8) | b);
}

void ContentLoader::WritePixelsToShaderIndex(uint32_t *data, int width, int height, int index)
{
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = data;
	initData.SysMemPitch = width * 4;
	initData.SysMemSlicePitch = width * height * 4;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;

	Engine::device->CreateTexture2D(&desc, &initData, tex.GetAddressOf());

	Engine::device->CreateShaderResourceView(tex.Get(), NULL, ContentLoader::GetTextureAddress(index));
}

void ContentLoader::PresentWindow(int index)
{
	m_index = index;
	GameTime::ResetWindowTimeStamp();
}

void ContentLoader::ClearWindow()
{
	m_index = -1;
	s_index = -1;
	GameTime::ResetWindowTimeStamp();
}

void ContentLoader::PresentOverlay(int index)
{
	s_index = index;
}

void ContentLoader::LoadContentStage(int stage)
{
	LoadHeaderInformation(stage);
	XGameInput::ResetHoldTracking();
}

ContentWindow& ContentLoader::GetCurrentWindow()
{
	return interfaces[m_index];
}

ContentOverlay& ContentLoader::GetCurrentOverlay()
{
	return overlays[s_index];
}

ID3D11ShaderResourceView** ContentLoader::GetTextureAddress(int index)
{
	return &texture_resources[index];
}

/* 
 * Hardcoded Interfaces For Now :(
 * Don't worry we'll have a content builder soon enough.
 */
void ContentLoader::LoadHeaderInformation(int stage)
{
	if (stage == 0)
		LoadMenuStage();
	else if (stage == 1)
		LoadWorldStage();
	else if (stage == 2)
		LoadSimple2DWorld();
}

/*
* Param 1 = Interfaces, Param 2 = Overlays, Param 3 = Fonts, Param 4 = Textures.
*/
void RestartHeaderSize(int total_interfaces, int total_overlays, int total_fonts, int total_textures)
{
	if (max_interfaces != 0)
		delete[] interfaces;

	if (max_fonts != 0)
		delete[] fonts;

	FontResource::ResetIncrementer();

	if (max_textures != 0)
	{
		for (int i = 0; i < max_textures; ++i)
			texture_resources[i]->Release();
	}

	if (max_overlays != 0)
		delete[] overlays;

	interfaces = new ContentWindow[total_interfaces];
	max_interfaces = total_interfaces;

	fonts = new FontResource[total_fonts];
	max_fonts = total_fonts;

	texture_resources = new ID3D11ShaderResourceView*[total_textures];
	max_textures = total_textures;

	overlays = new ContentOverlay[total_overlays];
	max_overlays = total_overlays;
}

void ContentLoader::AllocateVertexBuffers()
{
	D3D11_BUFFER_DESC bd = { 0 };
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = 0;

	bd.ByteWidth = sizeof(Vertex32Byte) * 32768;
	Engine::device->CreateBuffer(&bd, NULL, static_mesh_buffer.GetAddressOf());

	bd.ByteWidth = sizeof(Vertex32Byte) * 4096;
	Engine::device->CreateBuffer(&bd, NULL, static_interfaces_buffer.GetAddressOf());

	bd.ByteWidth = sizeof(Vertex32Byte) * 2000;
	Engine::device->CreateBuffer(&bd, NULL, static_overlay_buffer.GetAddressOf());
}

void ContentLoader::LoadSimple2DWorld()
{
	RestartHeaderSize(1, 1, 1, 2);

	CreateWICTextureFromFile(Engine::device.Get(), Engine::context.Get(), L"Assets/0_FONT.png", nullptr, &texture_resources[0], 0);
	fonts[0].CreateGlyphMapping(0);

	//Create Single White Pixel.
	uint32_t data[1] = { (uint32_t)255 | (uint32_t)(255 << 8) | (uint32_t)(255 << 16) | (uint32_t)(255 << 24) };
	WritePixelsToShaderIndex(data, 1, 1, 1);

	Float3 v = { CreateShaderColor(1.0f, 1.0f), 1.0f, 1.0f }; //White

	ContentOverlay &overlay = overlays[0];
	{
		//Reset Overlay Index.
		static_overlay_buffer_size = 0;

		//Set Overlay Update Index and Total Texture Count.
		overlay.SetUpdateProc(3);

		//Set First Texture Used.
		//Then Add Data To Present.
		static_overlay_buffer_size = fonts[0].AddStringToBuffer("2D Environement Overlay Testing", OverlayVerts, v, static_overlay_buffer_size, 0, 50);
		static_overlay_buffer_size = fonts[0].AddStringToBuffer("Hold ; To Return To Menu.", OverlayVerts, v, static_overlay_buffer_size, 200, 90);

		overlay.total_textures = 1;
		overlay.texture_index[0] = 0;
		overlay.offsets[0] = static_overlay_buffer_size;
	}

	ContentWindow &window = interfaces[0];
	{
		int begin = 6;
		static_interface_buffer_size = 6;
		v = { CreateShaderColor(1.0f, 1.0f), 1.0f, 0.0F }; //Yellow.
		static_interface_buffer_size = 
			XModelMesh::CreateTexturedSquare(InterfaceVerts, static_interface_buffer_size, v, 100, 100, 500, 500);

		v = { CreateShaderColor(1.0f, 1.0f), 0.0f, 1.0F }; //Purple.
		static_interface_buffer_size = 
			XModelMesh::CreateTexturedSquare(InterfaceVerts, static_interface_buffer_size, v, 100, 100, 800, 500);

		interfaces[0].state_changes = 1;
		interfaces[0].state_change_alias[0] = 1;
		interfaces[0].state_vertex_offsets[0] = begin;
		interfaces[0].state_vertex_sizes[0] = static_interface_buffer_size - begin;
		interfaces[0].SetUpdateProc(3);
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	Engine::context->Map(static_overlay_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &OverlayVerts, sizeof(Vertex32Byte) * static_overlay_buffer_size);
	Engine::context->Unmap(static_overlay_buffer.Get(), 0);

	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	Engine::context->Map(static_interfaces_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &InterfaceVerts, sizeof(Vertex32Byte) * static_interface_buffer_size);
	Engine::context->Unmap(static_interfaces_buffer.Get(), 0);

	PresentWindow(0);
	PresentOverlay(0);
}

void ContentLoader::LoadWorldStage()
{
	RestartHeaderSize(0, 1, 1, 4);

	CreateWICTextureFromFile(Engine::device.Get(), Engine::context.Get(), L"Assets/SMILEY512.png", nullptr, &texture_resources[0], 0);
	CreateWICTextureFromFile(Engine::device.Get(), Engine::context.Get(), L"Assets/RGB_TransparencyMap.png", nullptr, &texture_resources[1], 0);
	CreateWICTextureFromFile(Engine::device.Get(), Engine::context.Get(), L"Assets/3_FONT.png", nullptr, &texture_resources[2], 0);
	CreateWICTextureFromFile(Engine::device.Get(), Engine::context.Get(), L"Assets/FloorTiles.PNG", nullptr, &texture_resources[3], 0);
	
	Engine::context->GenerateMips(texture_resources[0]);
	Engine::context->GenerateMips(texture_resources[1]);

	fonts[0].CreateGlyphMapping(3);

	Float3 Color = {CreateShaderColor(0.05f, 1.0f), 0.15f, 0.05f};

	float
		xPos =   -96.0f,
		yPos =    0.00f,
		zPos =   -96.0f,
		SIZE =   288.0f,
		SIZE_2 =  72.0f;
	
	 MeshVerts[0] = { xPos			, yPos , zPos			, Color._1, Color._2, Color._3,	  0.0f,   0.0f   };
	 MeshVerts[1] = { xPos			, yPos , zPos +  SIZE	, Color._1, Color._2, Color._3,   0.0f,   SIZE_2 };
	 MeshVerts[2] = { xPos +  SIZE	, yPos , zPos			, Color._1, Color._2, Color._3,   SIZE_2, 0.0f   };

	 MeshVerts[3] = { xPos +  SIZE	, yPos , zPos			, Color._1, Color._2, Color._3,   SIZE_2, 0.0f   };
	 MeshVerts[4] = { xPos			, yPos , zPos +  SIZE	, Color._1, Color._2, Color._3,   0.0f,   SIZE_2 };
	 MeshVerts[5] = { xPos +  SIZE	, yPos , zPos +  SIZE	, Color._1, Color._2, Color._3,	  SIZE_2, SIZE_2 };

		xPos =   0.0f,
		yPos =   0.01f,
		zPos =   0.0f,
		SIZE =   96.0f,
		SIZE_2 = 96.0f;

		Color._1 = CreateShaderColor(0.4f, 1.0f), Color._2 = 0.15F, Color._3 = 0.15F;

	 MeshVerts[6] = { xPos			, yPos , zPos			, Color._1, Color._2, Color._3,	  0.0f,   0.0f };
	 MeshVerts[7] = { xPos			, yPos , zPos + SIZE	, Color._1, Color._2, Color._3,   0.0f,   SIZE_2 };
	 MeshVerts[8] = { xPos + SIZE		, yPos , zPos			, Color._1, Color._2, Color._3,   SIZE_2, 0.0f };

	 MeshVerts[9] = { xPos + SIZE		, yPos , zPos			, Color._1, Color._2, Color._3,   SIZE_2, 0.0f };
	MeshVerts[10] = { xPos			, yPos , zPos + SIZE	, Color._1, Color._2, Color._3,   0.0f,   SIZE_2 };
	MeshVerts[11] = { xPos + SIZE		, yPos , zPos + SIZE	, Color._1, Color._2, Color._3,	  SIZE_2, SIZE_2 };

	static_mesh_buffer_size = 11;

	XModelMesh::LoadCollisionData();
	XModelMesh::LoadObjectDefintions();

	for (int x = 1; x < 48; ++x)
	{
		for (int z = 1; z < 10; ++z)
		{
			XModelMesh::InsertObjectToMap(MeshVerts, static_mesh_buffer_size, 
				0, 100 + (x * 200), 100, 100 + (z * 300));
		}
	}

	Float3 v = { CreateShaderColor(1.0f, 1.0f), 1.0f, 1.0f};

	//Reset Overlay Index.
	static_overlay_buffer_size = 0;

	//Set Overlay Update Index and Total Texture Count.
	ContentOverlay &ow = overlays[0];

	ow.SetUpdateProc(2);
	ow.total_textures = 2;

	//Set First Texture Used.
	//Then Add Data To Present.
	ow.texture_index[0] = 2;

	int drawX = fonts[0].AddStringToOverlay("PositionX: ", 12, OverlayVerts, ow, static_overlay_buffer_size, v, 0, 60);
	fonts[0].AddStringToOverlay("", 12, OverlayVerts, ow, static_overlay_buffer_size, v, drawX, 60);

	drawX = fonts[0].AddStringToOverlay("PositionY: ", 12, OverlayVerts, ow, static_overlay_buffer_size, v, 0, 100);
	fonts[0].AddStringToOverlay("", 12, OverlayVerts, ow, static_overlay_buffer_size, v, drawX, 100);

	drawX = fonts[0].AddStringToOverlay("PositionZ: ", 12, OverlayVerts, ow, static_overlay_buffer_size, v, 0, 140);
	fonts[0].AddStringToOverlay("", 12, OverlayVerts, ow, static_overlay_buffer_size, v, drawX, 140);

	drawX = fonts[0].AddStringToOverlay("PromptViewID: ", 12, OverlayVerts, ow, static_overlay_buffer_size, v, 0, 180);
	fonts[0].AddStringToOverlay("", 12, OverlayVerts, ow, static_overlay_buffer_size, v, drawX, 180);

	drawX = fonts[0].AddStringToOverlay("PromptViewMin: ", 16, OverlayVerts, ow, static_overlay_buffer_size, v, 0, 220);
	fonts[0].AddStringToOverlay("", 12, OverlayVerts, ow, static_overlay_buffer_size, v, drawX, 220);

	drawX = fonts[0].AddStringToOverlay("PromptViewMax: ", 16, OverlayVerts, ow, static_overlay_buffer_size, v, 0, 260);
	fonts[0].AddStringToOverlay("", 12, OverlayVerts, ow, static_overlay_buffer_size, v, drawX, 260);

	//Set The Next Texture Used.
	//Set The Offset From Last Data.
	ow.texture_index[1] = 0;
	ow.offsets[0] = static_overlay_buffer_size;

	v = { CreateShaderColor(1.0f, 0.9f), 1.0f, 1.0f }; //Blocking Box.
	static_overlay_buffer_size = XModelMesh::CreateTexturedSquare(OverlayVerts, static_overlay_buffer_size, v, 1, 1, 500, 500);


	D3D11_MAPPED_SUBRESOURCE resource;
	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	Engine::context->Map(static_mesh_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &MeshVerts, sizeof(Vertex32Byte) * static_mesh_buffer_size);
	Engine::context->Unmap(static_mesh_buffer.Get(), 0);

	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	Engine::context->Map(static_overlay_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &OverlayVerts, sizeof(Vertex32Byte) * static_overlay_buffer_size);
	Engine::context->Unmap(static_overlay_buffer.Get(), 0);

	PresentOverlay(0);
	ALLOW_3D_PROCESSING = true;
}

void ContentLoader::LoadMenuStage()
{
	RestartHeaderSize(3, 0, 4, 7);

	InterfaceVerts[0] = { -1.0F,  1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	0.0F, 0.0F};
	InterfaceVerts[1] = {  1.0F, -1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	1.0F, 1.0F};
	InterfaceVerts[2] = { -1.0F, -1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	0.0F, 1.0F};

	InterfaceVerts[3] = {  1.0F, -1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	1.0F, 1.0F};
	InterfaceVerts[4] = { -1.0F,  1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	0.0F, 0.0F};
	InterfaceVerts[5] = {  1.0F,  1.0F,  0.0f,		1.0F, 1.0F, 1.0F,	1.0F, 0.0F};
	
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

	Float3 v = { CreateShaderColor(1.0f, 1.0f), 1.0f, 1.0f};

	int begin = 6;
	int offset = fonts[1].AddStringToBuffer("Developed By", InterfaceVerts, v, 6, 0, 50);
	offset = fonts[1].AddStringToBuffer("Jeremy DX", InterfaceVerts, v, offset, 0, 300);

	interfaces[0].state_changes = 1;
	interfaces[0].state_change_alias[0] = 2;
	interfaces[0].state_vertex_offsets[0] = begin;
	interfaces[0].state_vertex_sizes[0] = offset - begin;
	interfaces[0].SetUpdateProc(1);

	begin = offset;

	offset = fonts[3].AddStringToBuffer("This Engine Is In Testing Phase", InterfaceVerts, v, offset, 0, 100);
	offset = fonts[3].AddStringToBuffer("\n\nCurrently testing Content Loading", InterfaceVerts, v, offset, 0, 100);
	offset = fonts[3].AddStringToBuffer("\n\n\n\nYou can press any button to continue.", InterfaceVerts, v, offset, 0, 100);

	offset = fonts[3].AddStringToBuffer("Project Source: https://github.com/JeremyDX/DX_B ", InterfaceVerts, v, offset, 0, 450);
	offset = fonts[3].AddStringToBuffer("YouTube Channel: https://www.youtube.com/user/DxXNA/", InterfaceVerts, v, offset, 0, 500);

	interfaces[1].state_changes = 1;
	interfaces[1].state_change_alias[0] = 4;
	interfaces[1].state_vertex_offsets[0] = begin;
	interfaces[1].state_vertex_sizes[0] = offset - begin;
	interfaces[1].SetUpdateProc(1);

	begin = offset;

	Float3 Green = { CreateShaderColor(0.0f, 1.0f), 1.0f, 0.0F };
	offset = fonts[1].AddStringToBuffer("Undead Survival", InterfaceVerts, v, offset, 0, 10);
	offset = fonts[1].AddStringToBuffer("Game Engine", InterfaceVerts, v, offset, 0, 130);

	interfaces[2].state_vertex_offsets[0] = begin;
	interfaces[2].state_vertex_sizes[0] = offset - begin;

	begin = offset;

	offset = XModelMesh::CreateTexturedSquare(InterfaceVerts, offset, Green, 360, 360, 50, 300);
	offset = XModelMesh::CreateTexturedSquare(InterfaceVerts, offset, v, 360, 360, 460, 300);
	offset = XModelMesh::CreateTexturedSquare(InterfaceVerts, offset, v, 360, 360, 870, 300);

	interfaces[2].state_vertex_offsets[1] = begin;
	interfaces[2].state_vertex_sizes[1] = offset - begin;
	
	interfaces[2].state_changes = 2;
	interfaces[2].state_change_alias[0] = 2; //Title Text.
	interfaces[2].state_change_alias[1] = 6; //Bottom Images.
	
	interfaces[2].SetUpdateProc(2); //Contains Menu's.
	interfaces[2].SetChildUpdateProc(1, 3, 0); //Left/Right Menu Size 3. No Disabled Bits.

	D3D11_MAPPED_SUBRESOURCE resource;
	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	Engine::context->Map(static_interfaces_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &InterfaceVerts, sizeof(Vertex32Byte) * offset);
	Engine::context->Unmap(static_interfaces_buffer.Get(), 0);
}

void ContentLoader::RotateOverlayTexture(int begin, Float2 verts[4])
{
	//0 = Tri 1 - Top Left , 0
	//1 = Tri 1 - Bottom Right , 2
	//2 = Tri 1 - Bottom Left , 3

	//3 = Tri 2 - Bottom Right , 2
	//4 = Tri 2 - Top Left , 0
	//5 = Tri 2 - Top Right , 1

	OverlayVerts[begin + 0]._X = verts[1]._1 / ScreenManager::ASPECT_RATIO;
	OverlayVerts[begin + 1]._X = verts[3]._1 / ScreenManager::ASPECT_RATIO;
	OverlayVerts[begin + 2]._X = verts[2]._1 / ScreenManager::ASPECT_RATIO;

	OverlayVerts[begin + 3]._X = verts[3]._1 / ScreenManager::ASPECT_RATIO;
	OverlayVerts[begin + 4]._X = verts[1]._1 / ScreenManager::ASPECT_RATIO;
	OverlayVerts[begin + 5]._X = verts[0]._1 / ScreenManager::ASPECT_RATIO;

	OverlayVerts[begin + 0]._Y = verts[1]._2;
	OverlayVerts[begin + 1]._Y = verts[3]._2;
	OverlayVerts[begin + 2]._Y = verts[2]._2;

	OverlayVerts[begin + 3]._Y = verts[3]._2;
	OverlayVerts[begin + 4]._Y = verts[1]._2;
	OverlayVerts[begin + 5]._Y = verts[0]._2;
}

void ContentLoader::UpdateOverlayString(int children_id, const char* text)
{
	int value = overlays[s_index].components[children_id];

	FontResource &font = fonts[value >> 21];

	font.UpdateBufferString(value - ((value >> 21) << 21) , text, OverlayVerts);
}

void ContentLoader::SwapQuadsPosition(int offset_a, int offset_b)
{
	Float3 W = { CreateShaderColor(1.0f, 1.0f), 1.0f, 1.0f };
	Float3 G = { CreateShaderColor(0.0f, 1.0f), 1.0f, 0.0f };

	for (int a = 0; a < 3; ++a)
	{
		InterfaceVerts[offset_a]._1 = W._1;
		InterfaceVerts[offset_a]._2 = W._2;
		InterfaceVerts[offset_a]._3 = W._3;
		++offset_a;
		InterfaceVerts[offset_a]._1 = W._1;
		InterfaceVerts[offset_a]._2 = W._2;
		InterfaceVerts[offset_a]._3 = W._3;
		++offset_a;
	}

	for (int a = 0; a < 3; ++a)
	{
		InterfaceVerts[offset_b]._1 = G._1;
		InterfaceVerts[offset_b]._2 = G._2;
		InterfaceVerts[offset_b]._3 = G._3;
		++offset_b;
		InterfaceVerts[offset_b]._1 = G._1;
		InterfaceVerts[offset_b]._2 = G._2;
		InterfaceVerts[offset_b]._3 = G._3;
		++offset_b;
	}
}

void ContentLoader::SendUpdatedBufferToGpu()
{
	D3D11_MAPPED_SUBRESOURCE resource;

	if (m_index >= 0)
	{
		ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		Engine::context->Map(static_interfaces_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, InterfaceVerts, 4096 * 32);
		Engine::context->Unmap(static_interfaces_buffer.Get(), 0);
	}

	if (s_index >= 0) 
	{
		ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		Engine::context->Map(static_overlay_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, OverlayVerts, 2000 * 32);
		Engine::context->Unmap(static_overlay_buffer.Get(), 0);
	}
}