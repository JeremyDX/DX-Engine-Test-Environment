#include "pch.h"
#include "Engine.h"


Engine::Engine()
{

}

Engine::~Engine()
{

}

// this function initializes and prepares Direct3D for use
void Engine::Initialize()
{
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
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // how the swap chain should be used
	scd.BufferCount = 2;                                  // a front buffer and a back buffer
	scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;              // the most common swap chain format
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;    // the recommended flip mode
	scd.SampleDesc.Count = 1;                             // disable anti-aliasing

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

	texd.Width = (UINT)Window->Bounds.Width;
	texd.Height = (UINT)Window->Bounds.Height;
	texd.ArraySize = 1;
	texd.MipLevels = 1;
	texd.SampleDesc.Count = 1;
	texd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	texd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	ComPtr<ID3D11Texture2D> zbuffertexture;
	device->CreateTexture2D(&texd, nullptr, &zbuffertexture);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	ZeroMemory(&dsvd, sizeof(dsvd));

	device->CreateDepthStencilView(zbuffertexture.Get(), &dsvd, &zbuffer);


	D3D11_DEPTH_STENCIL_DESC dsd = { 0 };
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;

	device->CreateDepthStencilState(&dsd, depthOnState.GetAddressOf());

	dsd.DepthEnable = false;

	device->CreateDepthStencilState(&dsd, depthOffState.GetAddressOf());


	D3D11_VIEWPORT viewport = { 0 };

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = Window->Bounds.Width;
	viewport.Height = Window->Bounds.Height;
	viewport.MinDepth = 0.0F;    // the closest an object can be on the depth buffer is 0.0
	viewport.MaxDepth = 1.0F;    // the farthest an object can be on the depth buffer is 1.0

	context->RSSetViewports(1, &viewport);

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

	device->CreateRasterizerState(&rd, &rasterizerstate);


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
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	
	//UnsureUnused. 
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	//Seems to be a situation I'm not utilziing at the moment.
	bd.IndependentBlendEnable = FALSE;

	//Per MSDN - Set To False if ONLY RenderTarget[0] is USED.
	bd.AlphaToCoverageEnable = FALSE;

	device->CreateBlendState(&bd, &blendstate);

	CreateGeometry();
	CreatePipeline();

	//CoreWindow^ Window = CoreWindow::GetForCurrentThread();    // get the window pointer
	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), (FLOAT)Window->Bounds.Width / (FLOAT)Window->Bounds.Height, 1, 1000);                                                        // the far view-plane

	camera.SetProjection(&matProjection);

	camera.InitializeCameraPosition();

	time = 0.0F;
	camX = 0;
	camY = 1;
}

void Engine::CreateGeometry()
{
	XMFLOAT3 color(0.0F, 0.0F, 0.5F);

	//model_mesh.CreateColoredSquare(device.Get(), &vertexbuffer, 10, 10, 550, 550, color);
	
	TextureResource resource;
	//resource.CreateFromFile(device.Get(), "Wood.png");
	model_mesh.CreateTexturedSquare(device.Get(), &vertexbuffer, 10, 100, resource);
	model_mesh.CreateCubeObject(device.Get(), 0.0F, 0.0F, 8.0F);
}

void Engine::CreatePipeline()
{
	Platform::Array<byte>^ VSFile = file_reader.ReadBytesFromFile("VertexShader.cso");

	Platform::Array<byte>^ PositionColor = file_reader.ReadBytesFromFile("PositionColor.cso");
	Platform::Array<byte>^ TexturePositon = file_reader.ReadBytesFromFile("TexturePosition.cso");

	device->CreateVertexShader(VSFile->Data, VSFile->Length, nullptr, &vertexshader);
	device->CreatePixelShader(TexturePositon->Data, TexturePositon->Length, nullptr, &pixelshader);

	context->VSSetShader(vertexshader.Get(), nullptr, 0);
	context->PSSetShader(pixelshader.Get(), nullptr, 0);

	D3D11_INPUT_ELEMENT_DESC ied[2];

	ied[0] = VertexStructureTypes::PositionTexture[0];
	ied[1] = VertexStructureTypes::PositionTexture[1];

	// create and set the input layout
	device->CreateInputLayout(ied, ARRAYSIZE(ied), VSFile->Data, VSFile->Length, &inputlayout);
	context->IASetInputLayout(inputlayout.Get());

	// define and set the constant buffer
	D3D11_BUFFER_DESC bd = { 0 };

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(HLSLBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	device->CreateBuffer(&bd, nullptr, &constantbuffer);

	context->VSSetConstantBuffers(0, 1, constantbuffer.GetAddressOf());

	CreateWICTextureFromFile(device.Get(), nullptr, L"Wood.png", nullptr, &texture, 0);
	CreateWICTextureFromFile(device.Get(), nullptr, L"RGB_TransparencyMap.png", nullptr, &trans_texture, 0);
	CreateWICTextureFromFile(device.Get(), nullptr, L"Wood.png", nullptr, &white_texture, 0);
}

// this function performs updates to the state of the game
void Engine::Update()
{
	XGameInput::LoadController();
	controller_input = XGameInput::GetButtonBitSet();
	bool updated = 0L;

	XGameInput::GetJoysticks();


	if (updated)
	{
		camera.InitializeCameraPosition();
	}
}

void Engine::BeginScene()
{

}

void Engine::Render()
{
	// set our new render target object as the active render target
	context->OMSetRenderTargets(1, rendertarget.GetAddressOf(), nullptr);

	context->OMSetDepthStencilState(depthOnState.Get(), 0);


	float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->OMSetBlendState(blendstate.Get(), BlendFactor, 0xffffffff);

	context->RSSetState(rasterizerstate.Get());

	//context->OMSetDepthStencilState(depthOffState.Get(), 0);

	// clear the back buffer to a deep blue
	float color[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
	HLSLBuffer buffer;
	buffer.view_matrix = XMMatrixIdentity();

	context->ClearRenderTargetView(rendertarget.Get(), color);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set the vertex buffer
	UINT stride = sizeof(VertexPositionTexture);
	UINT offset = 0;

	//XMMATRIX matRotate = XMMatrixRotationY(time);
	buffer.view_matrix = XMLoadFloat4x4(&camera.view_matrix) * XMLoadFloat4x4(&camera.projection);
	 // matRotate * matView * matProjection;

	//THIS IS THE VERTEXS TO BE DRAWN.
	context->IASetVertexBuffers(0, 1, model_mesh.vertexbuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(model_mesh.indexbuffer.Get(), DXGI_FORMAT_R16_UINT, 0);


	context->UpdateSubresource(constantbuffer.Get(), 0, 0, &buffer, 0, 0);

	static ID3D11ShaderResourceView* shader_resources1[]{ texture.Get(), texture.Get() };
	context->PSSetShaderResources(0, ARRAYSIZE(shader_resources1), shader_resources1);


	context->DrawIndexed(36, 0, 0);

	//Begin 2D Render..

	/*buffer.view_matrix = XMMatrixIdentity();
	//buffer.view_matrix = matRotate2;
	context->IASetVertexBuffers(0, 1, vertexbuffer.GetAddressOf(), &stride, &offset);
	context->UpdateSubresource(constantbuffer.Get(), 0, 0, &buffer, 0, 0);

	static ID3D11ShaderResourceView* shader_resources2[]{ trans_texture.Get(), trans_texture.Get() };
	context->PSSetShaderResources(0, ARRAYSIZE(shader_resources2), shader_resources2);

	context->Draw(3, 0);*/

	// switch the back buffer and the front buffer
	swapchain->Present(1, 0);
}