#include "pch.h"

#include "Engine.h"
#include "Constants.h"
#include "FileReaderWriter.h"
#include "CameraEngine.h"
#include "XGameInput.h"
#include "XModelMesh.h"
#include "ContentLoader.h"
#include "GameTime.h"
#include "Animation.h"
#include "ScreenManager.h"

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace Windows::Graphics::Display;

static int ENGINE_ERROR_CODE = 0x0;
static const float VSYNC_ON = 1;
static const float BackgroundColor[4] = {0.05f, 0.0f, 0.0f, 1.0f};

ComPtr<ID3D11DeviceContext1> Engine::context;

ComPtr<ID3D11Device1> Engine::device;

ComPtr<IDXGISwapChain1> swapchain;

ComPtr<ID3D11RenderTargetView> rendertarget;

ComPtr<ID3D11VertexShader> vertexshader;
ComPtr<ID3D11PixelShader> pixelshader;
ComPtr<ID3D11InputLayout> inputlayout;

ComPtr<ID3D11Buffer> d3d_const_buffer;
ComPtr<ID3D11Buffer> d2d_const_buffer;

ComPtr<ID3D11Buffer> vertexbuffer;

ComPtr<ID3D11DepthStencilView> zbuffer;

ComPtr<ID3D11RasterizerState> rasterizerstate;

ComPtr<ID3D11BlendState> blendstate;

ComPtr<ID3D11DepthStencilState> depthonstate;
ComPtr<ID3D11DepthStencilState> depthoffstate;

ComPtr<ID3D11SamplerState> sampler;

FileReaderWriter file_reader;
CameraEngine camera;
XModelMesh model_mesh;

void CreatePipeline()
{
	Platform::Array<byte>^ VSFile = file_reader.ReadBytesFromFile("VertexShader.cso");

	Platform::Array<byte>^ PixelShader = file_reader.ReadBytesFromFile("TexturePosition.cso");

	Engine::device->CreateVertexShader(VSFile->Data, VSFile->Length, nullptr, &vertexshader);
	Engine::device->CreatePixelShader(PixelShader->Data, PixelShader->Length, nullptr, &pixelshader);

	Engine::context->VSSetShader(vertexshader.Get(), nullptr, 0);
	Engine::context->PSSetShader(pixelshader.Get(), nullptr, 0);

	// create and set the input layout
	Engine::device->CreateInputLayout(Constants::Layout_Byte32, 3, VSFile->Data, VSFile->Length, &inputlayout);
	Engine::context->IASetInputLayout(inputlayout.Get());

	// define and set the constant buffer
	D3D11_BUFFER_DESC bd = { 0 };

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = 64U;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	Engine::device->CreateBuffer(&bd, nullptr, &d3d_const_buffer);

	DirectX::XMFLOAT4X4 IDENTITY =
	{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	D3D11_SUBRESOURCE_DATA view_srd = { &IDENTITY, 0, 0 };

	Engine::device->CreateBuffer(&bd, &view_srd, &d2d_const_buffer);

	Engine::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void InitializeDirectXProperties()
{
	DisplayInformation ^display = DisplayInformation::GetForCurrentView();
	ScreenManager::UpdateScreenResolution(display->ScreenHeightInRawPixels, display->ScreenWidthInRawPixels);

	ComPtr<ID3D11Device> temp_device;
	ComPtr<ID3D11DeviceContext> temp_context;

	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION,
		&temp_device, nullptr, &temp_context);

	temp_device.As(&Engine::device);
	temp_context.As(&Engine::context);

	ComPtr<IDXGIDevice1> dxgiDevice;
	Engine::device.As(&dxgiDevice);

	// Second, use the IDXGIDevice1 interface to get access to the adapter
	ComPtr<IDXGIAdapter> dxgiAdapter;
	dxgiDevice->GetAdapter(&dxgiAdapter);

	// Third, use the IDXGIAdapter interface to get access to the factory
	ComPtr<IDXGIFactory2> dxgiFactory;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), &dxgiFactory);

	// set up the swap chain description
	DXGI_SWAP_CHAIN_DESC1 scd = { 0 };
	scd.Width = ScreenManager::PREFERRED_CANVAS_WIDTH;
	scd.Height = ScreenManager::PREFERRED_CANVAS_HEIGHT;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // how the swap chain should be used
	scd.BufferCount = 2;                                  // a front buffer and a back buffer
	scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;              // the most common swap chain format
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;    // the recommended flip mode
	scd.SampleDesc.Count = 1;                             // disable anti-aliasing
	//scd.Windowed = false;

	CoreWindow^ Window = CoreWindow::GetForCurrentThread();    // get the window pointer

	dxgiFactory->CreateSwapChainForCoreWindow(
		Engine::device.Get(),								// address of the Engine::device
		reinterpret_cast<IUnknown*>(Window),        // address of the window
		&scd,                                       // address of the swap chain description
		nullptr,                                    // advanced
		&swapchain);                                // address of the new swap chain pointer

	// get a pointer directly to the back buffer
	ComPtr<ID3D11Texture2D> backbuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer);

	// create a render target pointing to the back buffer
	Engine::device->CreateRenderTargetView(backbuffer.Get(), nullptr, &rendertarget);

	D3D11_TEXTURE2D_DESC texd = { 0 };

	texd.Width = ScreenManager::PREFERRED_CANVAS_WIDTH;
	texd.Height = ScreenManager::PREFERRED_CANVAS_HEIGHT;
	texd.ArraySize = 1;
	texd.MipLevels = 1;
	texd.SampleDesc.Count = 1;
	texd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ComPtr<ID3D11Texture2D> zbuffertexture;

	Engine::device->CreateTexture2D(&texd, nullptr, &zbuffertexture);

	Engine::device->CreateDepthStencilView(zbuffertexture.Get(), NULL, &zbuffer);

	D3D11_VIEWPORT viewport = { 0 };

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = ScreenManager::PREFERRED_CANVAS_WIDTH;
	viewport.Height = ScreenManager::PREFERRED_CANVAS_HEIGHT;
	viewport.MinDepth = 0.0F;
	viewport.MaxDepth = 1.0F;

	Engine::context->RSSetViewports(1, &viewport);

	D3D11_RASTERIZER_DESC rd;
	rd.CullMode = D3D11_CULL_NONE;
	rd.FrontCounterClockwise = TRUE;
	rd.DepthClipEnable = TRUE;
	rd.ScissorEnable = FALSE;
	rd.AntialiasedLineEnable = FALSE;
	rd.MultisampleEnable = FALSE;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.0f;
	rd.SlopeScaledDepthBias = 0.0f;

	Engine::device->CreateRasterizerState(&rd, &rasterizerstate);

	Engine::context->RSSetState(rasterizerstate.Get());

	D3D11_BLEND_DESC bd;

	//Allow Blending :)
	bd.RenderTarget[0].BlendEnable = TRUE;

	//Set the Source RGB * Source Alpha.
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;

	//Set the Destination.RGB * (1 - Source.Alpha).
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	//When Comparing the BlendSrc and BlendDest Add Them Together.
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	//When Comparing the SrcAlpha and DestAlpha Find which is greater. (Should always be destination based on render order).
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;

	//No Operations just return the SrcAlpha.
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;

	//No Operations just return the DestAlpha.
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_SRC_ALPHA;

	//UnsureUnused. 
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//Seems to be a situation I'm not utilziing at the moment.
	bd.IndependentBlendEnable = FALSE;

	//Per MSDN - Set To False if ONLY RenderTarget[0] is USED.
	bd.AlphaToCoverageEnable = FALSE;

	Engine::device->CreateBlendState(&bd, &blendstate);

	D3D11_DEPTH_STENCIL_DESC dsd = { 0 };

	dsd.DepthEnable = true;

	Engine::device->CreateDepthStencilState(&dsd, depthonstate.GetAddressOf());

	dsd.DepthEnable = false;

	Engine::device->CreateDepthStencilState(&dsd, depthoffstate.GetAddressOf());

	D3D11_SAMPLER_DESC samplerDesc;

	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	Engine::device->CreateSamplerState(&samplerDesc, sampler.GetAddressOf());

	CreatePipeline();

	CameraEngine::ResetPrimaryCameraMatrix(90);

	ContentLoader::AllocateVertexBuffers();
	ContentLoader::LoadContentStage(0); //First Content Batch.
	ContentLoader::PresentWindow(0);    //Show Us The First Screen!!

	Animation::LoadAnimations();

	GameTime::Begin(); //It's SAFE! to PresentWindow before we begin ticking. It'll initalize as Frame Index 0.
}


void Update()
{
	XGameInput::LoadController();

	//Handle Updates to Window if we have one open.
	if (ContentLoader::m_index >= 0)
	{
		ContentWindow cw = ContentLoader::GetCurrentWindow();
		cw.update();
	}

	if (ContentLoader::s_index >= 0)
	{
		ContentOverlay co = ContentLoader::GetCurrentOverlay();
		co.update();
	}

	//Update 3D Camera World Space Context.
	//Process Incoming Camera Change Data.
	//Automatic False Return if NO 3D IS IN USE!
	if (ContentLoader::ALLOW_3D_PROCESSING && CameraEngine::PrimaryCameraUpdatedLookAt())
	{
		Engine::context->UpdateSubresource(d3d_const_buffer.Get(), 0, 0, &CameraEngine::final_result, 0, 0);
	}

	//Submit Fresh Buffer To GPU.
	ContentLoader::SendUpdatedBufferToGpu();
}

UINT stride = sizeof(Vertex32Byte);
UINT offset = 0;

void Render()
{
	//Set the RenderTarget to the Swapped Buffer So We Can Draw To It!
	Engine::context->ClearDepthStencilView(zbuffer.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	Engine::context->OMSetRenderTargets(1, rendertarget.GetAddressOf(), zbuffer.Get());
	Engine::context->ClearRenderTargetView(rendertarget.Get(), BackgroundColor);

	if (ContentLoader::ALLOW_3D_PROCESSING)
	{
		Engine::context->PSSetShaderResources(0, 1, ContentLoader::GetTextureAddress(0));
		Engine::context->PSSetSamplers(0, 1, sampler.GetAddressOf());

		Engine::context->UpdateSubresource(d3d_const_buffer.Get(), 0, 0, &CameraEngine::final_result, 0, 0);
		Engine::context->OMSetDepthStencilState(depthonstate.Get(), 0);
		Engine::context->VSSetConstantBuffers(0, 1, d3d_const_buffer.GetAddressOf());

		Engine::context->IASetVertexBuffers(0, 1, ContentLoader::static_mesh_buffer.GetAddressOf(), &stride, &offset);
		Engine::context->Draw(ContentLoader::static_mesh_buffer_size, 0);
	}

	//Draw 2D.
	if (ContentLoader::m_index >= 0)
	{
		Engine::context->OMSetDepthStencilState(depthoffstate.Get(), 0);
		Engine::context->VSSetConstantBuffers(0, 1, d2d_const_buffer.GetAddressOf());

		float BlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		Engine::context->OMSetBlendState(blendstate.Get(), BlendFactor, 0xFFFFFFFF);

		ContentWindow cw = ContentLoader::GetCurrentWindow();

		Engine::context->IASetVertexBuffers(0, 1, ContentLoader::static_interfaces_buffer.GetAddressOf(), &stride, &offset);

		if (cw.background_shader_id >= 0)
		{
			//CODE SECITON #1
			Engine::context->PSSetShaderResources(0, 1, ContentLoader::GetTextureAddress(cw.background_shader_id));
			Engine::context->Draw(6, 0);
		}
		for (int i = 0; i < cw.state_changes; ++i)
		{
			//CODE SECITON #2
			Engine::context->PSSetShaderResources(0, 1,
				ContentLoader::GetTextureAddress(cw.state_change_alias[i])
			);
			Engine::context->Draw(cw.state_vertex_sizes[i], cw.state_vertex_offsets[i]);
		}
	}

	if (ContentLoader::s_index >= 0)
	{
		Engine::context->OMSetDepthStencilState(depthoffstate.Get(), 0);
		Engine::context->VSSetConstantBuffers(0, 1, d2d_const_buffer.GetAddressOf());

		float BlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		Engine::context->OMSetBlendState(blendstate.Get(), BlendFactor, 0xFFFFFFFF);

		ContentOverlay co = ContentLoader::GetCurrentOverlay();

		Engine::context->IASetVertexBuffers(0, 1, ContentLoader::static_overlay_buffer.GetAddressOf(), &stride, &offset);

		Engine::context->PSSetShaderResources(0, 1, ContentLoader::GetTextureAddress(co.texture_index[0]));
		Engine::context->Draw(co.offsets[0], 0);

		for (int i = 1; i < co.total_textures; ++i)
		{
			Engine::context->PSSetShaderResources(0, 1, ContentLoader::GetTextureAddress(co.texture_index[i]));
			Engine::context->Draw(co.offsets[i], co.offsets[i - 1]);
		}
	}

	swapchain->Present(VSYNC_ON, 0);
}

bool KEEP_LOOPING = false;

//Force Kill Program.
void Engine::Stop()
{
	KEEP_LOOPING = false;
}

//Only To Be Called Once. Secondary Call Will Kill Execution and End Program.
int Engine::Boot(int preferred_height, int preferred_width)
{
	KEEP_LOOPING = !KEEP_LOOPING;
	ScreenManager::UpdatePreferredCanvasSize(preferred_height, preferred_width);
	InitializeDirectXProperties();

	CoreWindow^ Window = CoreWindow::GetForCurrentThread();

	while (KEEP_LOOPING)
	{
		Window->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

		Update();
		Render();

		GameTime::Tick();
	}
	return 0;
}