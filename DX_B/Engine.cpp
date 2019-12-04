#include "pch.h"
#include "Engine.h"

#include "BinaryReaderWriter.h"
#include "CameraEngine.h"
#include "Constants.h"
#include "ContentLoader.h"
#include "ContentWindow.h"
#include "GameTime.h"
#include "XGameInput.h"

/*
 * 0x0 (0) = Load Engine As Normal.
 * 0x1 (1) = Enable Main File Cache Content Builder Tools.
 */
static const uint32_t BINARY_FILE_BUILDER_BITMASK = 0x1;

static const float	BACKGROUND_COLOR[4]	= { 0.05f, 0.0f, 0.0f, 1.0f };
static uint32_t		ENGINE_ERROR_CODE	= 0x1;

Microsoft::WRL::ComPtr<ID3D11DeviceContext1> Engine::context;
Microsoft::WRL::ComPtr<ID3D11Device1> Engine::device;
uint16_t Engine::PREFERRED_CANVAS_WIDTH = 0;
uint16_t Engine::PREFERRED_CANVAS_HEIGHT = 0;

static Microsoft::WRL::ComPtr<IDXGISwapChain1> swapchain;
static Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target;

static Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
static Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
static Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;

static Microsoft::WRL::ComPtr<ID3D11Buffer> 
	d3d_const_buffer,
	d2d_const_buffer,
	vertex_buffer;

static Microsoft::WRL::ComPtr<ID3D11DepthStencilView> 
	zbuffer;

static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> 
	depth_on_state, 
	depth_off_state;

static Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state;
static Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state;
static Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;

void CreateDirectXForUWP()
{
	Windows::UI::Core::CoreWindow^ window = Windows::UI::Core::CoreWindow::GetForCurrentThread();

	Microsoft::WRL::ComPtr<IDXGIDevice1> dxgi_device; 
	Microsoft::WRL::ComPtr<IDXGIAdapter> dxgi_adapter;
	Microsoft::WRL::ComPtr<IDXGIFactory2> dxgi_factory;

	Microsoft::WRL::ComPtr<ID3D11Device> temp_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> temp_context;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> zbuffer_texture;

	DXGI_SWAP_CHAIN_DESC1 swap_chain_description = { 0 };
	D3D11_TEXTURE2D_DESC texture_description = { 0 };

	swap_chain_description.Width =				Engine::PREFERRED_CANVAS_WIDTH;
	swap_chain_description.Height =				Engine::PREFERRED_CANVAS_HEIGHT;
	swap_chain_description.BufferUsage =		DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swap_chain_description.BufferCount =		2;
	swap_chain_description.Format =				DXGI_FORMAT_B8G8R8A8_UNORM;
	swap_chain_description.SwapEffect =			DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swap_chain_description.SampleDesc.Count =	1;

	texture_description.Width =				Engine::PREFERRED_CANVAS_WIDTH;
	texture_description.Height =			Engine::PREFERRED_CANVAS_HEIGHT;
	texture_description.ArraySize =			1;
	texture_description.MipLevels =			1;
	texture_description.SampleDesc.Count =	1;
	texture_description.Format =			DXGI_FORMAT_D24_UNORM_S8_UINT;
	texture_description.BindFlags =			D3D11_BIND_DEPTH_STENCIL;

	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &temp_device, nullptr, &temp_context);
	temp_device.As(&Engine::device);
	temp_context.As(&Engine::context);

	Engine::device.As(&dxgi_device);
	dxgi_device->GetAdapter(&dxgi_adapter);
	dxgi_adapter->GetParent(__uuidof(IDXGIFactory2), &dxgi_factory);

	dxgi_factory->CreateSwapChainForCoreWindow(Engine::device.Get(), reinterpret_cast<IUnknown*>(window), &swap_chain_description, nullptr, &swapchain);

	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &back_buffer);
	Engine::device->CreateRenderTargetView(back_buffer.Get(), nullptr, &render_target);

	Engine::device->CreateTexture2D(&texture_description, nullptr, &zbuffer_texture);
	Engine::device->CreateDepthStencilView(zbuffer_texture.Get(), NULL, &zbuffer);
}

void CreateDirectXDescriptions()
{
	D3D11_VIEWPORT viewport = { 0 };

	D3D11_BLEND_DESC blend_description = { 0 };
	D3D11_DEPTH_STENCIL_DESC depth_stencil_description = { 0 };

	D3D11_RASTERIZER_DESC rasterizer_description;
	D3D11_SAMPLER_DESC sampler_description;

	ZeroMemory(&rasterizer_description, sizeof(D3D11_RASTERIZER_DESC));
	ZeroMemory(&sampler_description,	sizeof(D3D11_SAMPLER_DESC));

	viewport.TopLeftX =		0;
	viewport.TopLeftY =		0;
	viewport.Width =		Engine::PREFERRED_CANVAS_WIDTH;
	viewport.Height =		Engine::PREFERRED_CANVAS_HEIGHT;
	viewport.MinDepth =		0.0F;
	viewport.MaxDepth =		1.0F;

	rasterizer_description.CullMode =				D3D11_CULL_NONE;
	rasterizer_description.FrontCounterClockwise =	TRUE;
	rasterizer_description.DepthClipEnable =		TRUE;
	rasterizer_description.ScissorEnable =			FALSE;
	rasterizer_description.AntialiasedLineEnable =	FALSE;
	rasterizer_description.MultisampleEnable =		FALSE;
	rasterizer_description.DepthBias =				0;
	rasterizer_description.DepthBiasClamp =			0.0f;
	rasterizer_description.SlopeScaledDepthBias =	0.0f;

	blend_description.RenderTarget[0].BlendEnable =				TRUE;
	blend_description.RenderTarget[0].SrcBlend =				D3D11_BLEND_SRC_ALPHA;
	blend_description.RenderTarget[0].DestBlend =				D3D11_BLEND_INV_SRC_ALPHA;
	blend_description.RenderTarget[0].BlendOp =					D3D11_BLEND_OP_ADD;
	blend_description.RenderTarget[0].BlendOpAlpha =			D3D11_BLEND_OP_MAX;
	blend_description.RenderTarget[0].SrcBlendAlpha =			D3D11_BLEND_SRC_ALPHA;
	blend_description.RenderTarget[0].DestBlendAlpha =			D3D11_BLEND_SRC_ALPHA;
	blend_description.RenderTarget[0].RenderTargetWriteMask =	D3D11_COLOR_WRITE_ENABLE_ALL;
	blend_description.IndependentBlendEnable =					FALSE;
	blend_description.AlphaToCoverageEnable =					FALSE;

	sampler_description.Filter =		 D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampler_description.AddressU =		 D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_description.AddressV =		 D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_description.AddressW =		 D3D11_TEXTURE_ADDRESS_WRAP;
	sampler_description.MipLODBias =	 0.0f;
	sampler_description.MaxAnisotropy =  16;
	sampler_description.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampler_description.BorderColor[0] = 0;
	sampler_description.BorderColor[1] = 0;
	sampler_description.BorderColor[2] = 0;
	sampler_description.BorderColor[3] = 0;
	sampler_description.MinLOD = 0;
	sampler_description.MaxLOD = D3D11_FLOAT32_MAX;

	Engine::context->RSSetViewports(1, &viewport);
	Engine::device->CreateRasterizerState(&rasterizer_description, &rasterizer_state);
	Engine::context->RSSetState(rasterizer_state.Get());
	Engine::device->CreateBlendState(&blend_description, &blend_state);

	depth_stencil_description.DepthEnable = true;
	Engine::device->CreateDepthStencilState(&depth_stencil_description, depth_on_state.GetAddressOf());

	depth_stencil_description.DepthEnable = false;
	Engine::device->CreateDepthStencilState(&depth_stencil_description, depth_off_state.GetAddressOf());

	Engine::device->CreateSamplerState(&sampler_description, sampler_state.GetAddressOf());
}


void CreateShaderPipelines()
{
	char* buffer_reference = NULL;
	int length = 0;
	BinaryReaderWriter::ReadBinaryData("VertexShader.cso", buffer_reference, length);
	Engine::device->CreateVertexShader(buffer_reference, length, nullptr, &vertex_shader);
	Engine::device->CreateInputLayout(Constants::Layout_Byte32, 3, buffer_reference, length, &input_layout);

	BinaryReaderWriter::ReadBinaryData("VertexShader.cso", buffer_reference, length);
	Engine::device->CreatePixelShader(buffer_reference, length, nullptr, &pixel_shader);

	Engine::context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	Engine::context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	Engine::context->IASetInputLayout(input_layout.Get());

	D3D11_BUFFER_DESC buffer_description;

	buffer_description.Usage = D3D11_USAGE_DEFAULT;
	buffer_description.ByteWidth = 64U;
	buffer_description.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	Engine::device->CreateBuffer(&buffer_description, nullptr, &d3d_const_buffer);

	DirectX::XMFLOAT4X4 IDENTITY =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	D3D11_SUBRESOURCE_DATA view_srd = { &IDENTITY, 0, 0 };

	Engine::device->CreateBuffer(&buffer_description, &view_srd, &d2d_const_buffer);

	Engine::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Clean Up Shader Memory.
	delete[] buffer_reference;
}


void Update()
{
	XGameInput::LoadController();

	//Update 3D Camera World Space Context.
	//Process Incoming Camera Change Data.
	//Automatic False Return if NO 3D IS IN USE!
	if ((ContentLoader::RENDER_BITMASK & 0x1) && CameraEngine::PrimaryCameraUpdatedLookAt())
	{
		Engine::context->UpdateSubresource(d3d_const_buffer.Get(), 0, 0, &CameraEngine::final_result, 0, 0);
	}

	ContentLoader::Update();
}

void Render()
{
	//Set the RenderTarget to the Swapped Buffer So We Can Draw To It!
	Engine::context->ClearDepthStencilView(zbuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	Engine::context->OMSetRenderTargets(1, render_target.GetAddressOf(), zbuffer.Get());
	Engine::context->ClearRenderTargetView(render_target.Get(), BACKGROUND_COLOR);

	UINT stride = sizeof(Vertex32Byte);
	UINT offset = 0;

	//Render 3D Content.
	//Render BitMask 1 = Allow 3D Rendering.
	if (ContentLoader::RENDER_BITMASK & 0x1)
	{
		Engine::context->PSSetShaderResources(0, 1, ContentLoader::GetTextureAddress(3));
		Engine::context->PSSetSamplers(0, 1, sampler_state.GetAddressOf());

		Engine::context->UpdateSubresource(d3d_const_buffer.Get(), 0, 0, &CameraEngine::final_result, 0, 0);
		Engine::context->OMSetDepthStencilState(depth_on_state.Get(), 0);
		Engine::context->VSSetConstantBuffers(0, 1, d3d_const_buffer.GetAddressOf());

		Engine::context->IASetVertexBuffers(0, 1, ContentLoader::static_mesh_buffer.GetAddressOf(), &stride, &offset);
		Engine::context->Draw(ContentLoader::static_mesh_buffer_size, 0);
	}

	//Render 2D Interfaces.
	if (ContentLoader::current_interface_id)
	{
		Engine::context->OMSetDepthStencilState(depth_off_state.Get(), 0);
		Engine::context->VSSetConstantBuffers(0, 1, d2d_const_buffer.GetAddressOf());

		float BlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		Engine::context->OMSetBlendState(blend_state.Get(), BlendFactor, 0xFFFFFFFF);

		ContentWindow& cw = ContentLoader::GetCurrentWindow();

		Engine::context->IASetVertexBuffers(0, 1, ContentLoader::static_interfaces_buffer.GetAddressOf(), &stride, &offset);

		//Render BitMask 2 = Allow Background Texture To Draw.
		if (ContentLoader::RENDER_BITMASK & 0x2)
		{
			Engine::context->PSSetShaderResources(0, 1, ContentLoader::GetTextureAddress(cw.texture_ids[0]));
			Engine::context->Draw(6, 0);
		}

		for (int i = 1; i < cw.texture_count; ++i)
		{
			Engine::context->PSSetShaderResources(0, 1, ContentLoader::GetTextureAddress(cw.texture_ids[i]));
			Engine::context->Draw(cw.vertex_offsets[i], cw.vertex_offsets[i - 1]);
		}
	}

	//Render 2D Overlay's (Doesn't Effect 3D Camera).
	if (ContentLoader::current_overlay_id)
	{
		Engine::context->OMSetDepthStencilState(depth_off_state.Get(), 0);
		Engine::context->VSSetConstantBuffers(0, 1, d2d_const_buffer.GetAddressOf());

		float BlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		Engine::context->OMSetBlendState(blend_state.Get(), BlendFactor, 0xFFFFFFFF);

		ContentWindow cw = ContentLoader::GetCurrentOverlay();

		Engine::context->IASetVertexBuffers(0, 1, ContentLoader::static_overlay_buffer.GetAddressOf(), &stride, &offset);

		Engine::context->PSSetShaderResources(0, 1, ContentLoader::GetTextureAddress(cw.texture_ids[0]));
		Engine::context->Draw(cw.vertex_offsets[0], 0);

		for (int i = 1; i < cw.texture_count; ++i)
		{
			Engine::context->PSSetShaderResources(0, 1, ContentLoader::GetTextureAddress(cw.texture_ids[i]));
			Engine::context->Draw(cw.vertex_offsets[i], cw.vertex_offsets[i - 1]);
		}
	}
	                                 
	swapchain->Present(true, 0); //Vsync Enabled.
}

/*
 * Force Kills The Program.
 */
void Engine::Stop()
{
	ENGINE_ERROR_CODE = 0x1;
}

/*
 * Only To Be Called Once. Secondary Call Will Kill Execution and End Program.
 * 0x0 = Program Started And Running.
 * 0x1 = Program Ended Safely.
 * 0x2 and above = Not Supported (Future Debugging System).
 */
 int Engine::Boot(int preferred_height, int preferred_width)
{	
	 if (!ENGINE_ERROR_CODE)
		 return 0x0;

	ENGINE_ERROR_CODE = 0;
	PREFERRED_CANVAS_WIDTH = preferred_width;
	PREFERRED_CANVAS_HEIGHT = preferred_height;

	CreateDirectXForUWP();
	CreateDirectXDescriptions();
	CreateShaderPipelines();

	/*
	 * Actual Process For Getting Game Started - Simple!
	 * Content Loader - Build Function Call System Before Loading Content or Presenting Windows.
	 * Content Loader - Begin's by loading all the data, variables, memory allocations, and class construction.
	 * Content Loader - Then we simply initialize the game with the first interface.
	 */
	ContentLoader::BuildFunctionCalls();
	ContentLoader::LoadContentStage(0);
	ContentLoader::PresentWindow(0);

	GameTime::Begin(); //It's SAFE! to PresentWindow before we begin ticking. It'll initalize as Frame Index 0.

	Windows::UI::Core::CoreWindow^ window = Windows::UI::Core::CoreWindow::GetForCurrentThread();
	while (ENGINE_ERROR_CODE == 0x0)
	{
		window->Dispatcher->ProcessEvents(Windows::UI::Core::CoreProcessEventsOption::ProcessAllIfPresent);
		
		Update();
		Render();

		GameTime::Tick();
	}
	return ENGINE_ERROR_CODE;
}