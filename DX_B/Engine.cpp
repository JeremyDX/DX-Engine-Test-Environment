#include "pch.h"
#include "Engine.h"

using namespace Windows::Graphics::Display;

static const float VSYNC_ON = 1;

static const float BackgroundColor[4] = {0.3f, 0.4f, 0.3f, 1.0f};

static const XMFLOAT4X4 IDENTITY_MATRIX
{
	1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1
};

void Engine::Initialize()
{
	DisplayInformation ^display = DisplayInformation::GetForCurrentView();
	ScreenManager::UpdateScreenResolution(display->ScreenHeightInRawPixels, display->ScreenWidthInRawPixels);

	ComPtr<ID3D11Device> temp_device;
	ComPtr<ID3D11DeviceContext> temp_context;

	D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, 
		&temp_device, nullptr, &temp_context);

	temp_device.As(&device);
	temp_context.As(&context);

	ComPtr<IDXGIDevice1> dxgiDevice;
	device.As(&dxgiDevice);

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
		device.Get(),								// address of the device
		reinterpret_cast<IUnknown*>(Window),        // address of the window
		&scd,                                       // address of the swap chain description
		nullptr,                                    // advanced
		&swapchain);                                // address of the new swap chain pointer

	// get a pointer directly to the back buffer
	ComPtr<ID3D11Texture2D> backbuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), &backbuffer);

	// create a render target pointing to the back buffer
	device->CreateRenderTargetView(backbuffer.Get(), nullptr, &rendertarget);

	D3D11_TEXTURE2D_DESC texd = { 0 };

	texd.Width = ScreenManager::PREFERRED_CANVAS_WIDTH;
	texd.Height = ScreenManager::PREFERRED_CANVAS_HEIGHT;
	texd.ArraySize = 1;
	texd.MipLevels = 1;
	texd.SampleDesc.Count = 1;
	texd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ComPtr<ID3D11Texture2D> zbuffertexture;

	device->CreateTexture2D(&texd, nullptr, &zbuffertexture);

	device->CreateDepthStencilView(zbuffertexture.Get(), NULL, &zbuffer);

	viewport = {0};

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = ScreenManager::PREFERRED_CANVAS_WIDTH;
	viewport.Height = ScreenManager::PREFERRED_CANVAS_HEIGHT;
	viewport.MinDepth = 0.0F; 
	viewport.MaxDepth = 1.0F;

	context->RSSetViewports(1, &viewport);

	D3D11_RASTERIZER_DESC rd;
	rd.CullMode = D3D11_CULL_FRONT;
	rd.FrontCounterClockwise = TRUE;
	rd.DepthClipEnable = TRUE;
	rd.ScissorEnable = FALSE;
	rd.AntialiasedLineEnable = FALSE;
	rd.MultisampleEnable = FALSE;
	rd.DepthBias = 0;
	rd.DepthBiasClamp = 0.0f;
	rd.SlopeScaledDepthBias = 0.0f;

	device->CreateRasterizerState(&rd, &rasterizerstate);

	context->RSSetState(rasterizerstate.Get());

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

	device->CreateBlendState(&bd, &blendstate);

	D3D11_DEPTH_STENCIL_DESC dsd = { 0 };

	dsd.DepthEnable = true;

	device->CreateDepthStencilState(&dsd, depthonstate.GetAddressOf());

	dsd.DepthEnable = false;

	device->CreateDepthStencilState(&dsd, depthoffstate.GetAddressOf());

	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, sampler.GetAddressOf());

	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), ScreenManager::ASPECT_RATIO, 1, 1000);                                                        // the far view-plane

	camera.SetProjection(&matProjection);
	camera.InitializeCameraPosition();

	CreatePipeline();

	ContentLoader::LoadContentStage(0); //First Content Batch.
	ContentLoader::PresentWindow(0);    //Show Us The First Screen!!
}

void Engine::CreatePipeline()
{
	Platform::Array<byte>^ VSFile = file_reader.ReadBytesFromFile("VertexShader.cso");

	Platform::Array<byte>^ PixelShader = file_reader.ReadBytesFromFile("TexturePosition.cso");

	device->CreateVertexShader(VSFile->Data, VSFile->Length, nullptr, &vertexshader);
	device->CreatePixelShader(PixelShader->Data, PixelShader->Length, nullptr, &pixelshader);

	context->VSSetShader(vertexshader.Get(), nullptr, 0);
	context->PSSetShader(pixelshader.Get(), nullptr, 0);

	// create and set the input layout
	device->CreateInputLayout(VertexStructureTypes::Layout_Byte32, 3, VSFile->Data, VSFile->Length, &inputlayout);
	context->IASetInputLayout(inputlayout.Get());

	// define and set the constant buffer
	D3D11_BUFFER_DESC bd = { 0 };

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(HLSLBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	
	device->CreateBuffer(&bd, nullptr, &d3d_const_buffer);

	HLSLBuffer viewbuffer;
	viewbuffer.view_matrix = IDENTITY_MATRIX;
	D3D11_SUBRESOURCE_DATA view_srd = { &viewbuffer, 0, 0 };

	device->CreateBuffer(&bd, &view_srd, &d2d_const_buffer);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void Engine::Update()
{
	XGameInput::LoadController();

	if (ContentLoader::m_index >= 0)
	{
		ContentWindow cw = ContentLoader::GetCurrentWindow();
		cw.update();
	} else {
		if (camera.ProcessCameraChanges())
		{
			//camera.UpdateViewMatrix();
			//HLSLBuffer buffer;
			//XMStoreFloat4x4(&buffer.view_matrix, (XMLoadFloat4x4(&camera.view_matrix) * XMLoadFloat4x4(&camera.projection)));
			//Engine::context->UpdateSubresource(d3d_const_buffer.Get(), 0, 0, &buffer, 0, 0);
		}
	}
}

void Engine::Render()
{
	//Set the RenderTarget to the Swapped Buffer So We Can Draw To It!
	context->ClearDepthStencilView(zbuffer.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->OMSetRenderTargets(1, rendertarget.GetAddressOf(), zbuffer.Get());
	context->ClearRenderTargetView(rendertarget.Get(), BackgroundColor);

	context->OMSetDepthStencilState(depthoffstate.Get(), 0);

	context->VSSetConstantBuffers(0, 1, d2d_const_buffer.GetAddressOf());
	if (ContentLoader::m_index >= 0)
	{

		UINT stride = sizeof(Vertex32Byte);
		UINT offset = 0;

		float BlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		context->OMSetBlendState(blendstate.Get(), BlendFactor, 0xFFFFFFFF);

		ContentWindow cw = ContentLoader::GetCurrentWindow();

		context->IASetVertexBuffers(0, 1, ContentLoader::static_interfaces_buffer.GetAddressOf(), &stride, &offset);

		if (cw.background_shader_id >= 0)
		{
			//CODE SECITON #1
			context->PSGetShaderResources(0, 1, ContentLoader::GetTextureAddress(cw.background_shader_id));
			context->Draw(6, 0);
		}
		for (int i = 0; i < cw.state_changes; ++i)
		{
			//CODE SECITON #2
			context->PSSetShaderResources(0, 1, 
				ContentLoader::GetTextureAddress(cw.state_change_alias[i])
			);
			context->Draw(cw.state_vertex_sizes[i], cw.state_vertex_offsets[i]);
		}

		//CODE SECITON #3
		context->IASetVertexBuffers(0, 1, ContentLoader::dynamic_interfaces_buffer.GetAddressOf(), &stride, &offset);
		context->PSSetShaderResources(0, 1, ContentLoader::GetTextureAddress(3));
		context->Draw(ContentLoader::dynamic_interface_buffer_size, 0);
	}

	swapchain->Present(VSYNC_ON, 0);
}

Engine::Engine() { }
Engine::~Engine() { }

ComPtr<ID3D11DeviceContext1> Engine::context;
ComPtr<ID3D11Device1> Engine::device;
D3D11_VIEWPORT Engine::viewport;