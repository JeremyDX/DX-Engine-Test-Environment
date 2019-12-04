#include "pch.h"
#include "ContentLoader.h"

#include "Constants.h"
#include "ContentWindow.h"
#include "GameTime.h"
#include "XGameInput.h"


static ContentWindow *interface_content;
static ContentWindow *overlay_content;

static ID3D11ShaderResourceView** texture_resources		= NULL;

Microsoft::WRL::ComPtr<ID3D11Buffer> 
	ContentLoader::static_interfaces_buffer,
	ContentLoader::static_overlay_buffer,
	ContentLoader::static_mesh_buffer;

uint32_t ContentLoader::current_interface_id			= 0;
uint32_t ContentLoader::current_overlay_id				= 0;
uint32_t ContentLoader::static_interface_buffer_size	= 0;
uint32_t ContentLoader::static_mesh_buffer_size			= 0;
uint32_t ContentLoader::static_overlay_buffer_size		= 0;

uint32_t ContentLoader::RENDER_BITMASK		= 0x0;

static uint16_t		interface_count			= 0;
static uint16_t		overlays_count			= 0;
static uint16_t		texture_count			= 0;
static uint16_t		font_defintions_count	= 0;

Vertex32Byte MeshVerts[32768];
Vertex32Byte InterfaceVerts[4096];
Vertex32Byte OverlayVerts[2000];

void ContentLoader::BuildFunctionCalls()
{

}

void ContentLoader::LoadContentStage(int stage)
{
	XGameInput::ResetHoldTracking();
}

void ContentLoader::Update()
{
	//Handle Updates to Window if we have one open.

	if (current_interface_id)
		interface_content[current_interface_id].update();
	
	if(current_overlay_id)
		overlay_content[current_interface_id].update();
}

void ContentLoader::PresentWindow(int index)
{
	current_interface_id = index;
	GameTime::ResetWindowTimeStamp();
}

void ContentLoader::PresentOverlay(int index)
{
	current_overlay_id = index;
}

void ContentLoader::FullyCloseAndResetWindows()
{
	current_interface_id = 0;
	current_overlay_id = 0;
	GameTime::ResetWindowTimeStamp();
}

ContentWindow& ContentLoader::GetCurrentWindow()
{
	return interface_content[current_interface_id];
}

ContentWindow& ContentLoader::GetCurrentOverlay()
{
	return overlay_content[current_overlay_id];
}

ID3D11ShaderResourceView** ContentLoader::GetTextureAddress(int index)
{
	return &texture_resources[index];
}