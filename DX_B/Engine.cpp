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

	device->CreateDepthStencilView(zbuffertexture.Get(), NULL, &zbuffer);

	///////////////////////// Map's Texture
// Initialize the  texture description.
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the texture description.
	// We will have our map be a square
	// We will need to have this texture bound as a render target AND a shader resource
	textureDesc.Width = 775;
	textureDesc.Height = 454;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the texture
	device->CreateTexture2D(&textureDesc, NULL, &zbuffertexture2);

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	/////////////////////// Map's Render Target
	// Setup the description of the render target view.
	renderTargetViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	device->CreateRenderTargetView(zbuffertexture2.Get(), &renderTargetViewDesc, alt_rendertarget.GetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	/////////////////////// Map's Shader Resource View
	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	device->CreateShaderResourceView(zbuffertexture2.Get(), &shaderResourceViewDesc, alt_shadertexture.GetAddressOf());

	viewport = { 0 };

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


	D3D11_DEPTH_STENCIL_DESC dsd = { 0 };
	dsd.DepthEnable = true;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc = D3D11_COMPARISON_LESS;

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

	CreateGeometry();
	CreatePipeline();

	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45), (FLOAT)Window->Bounds.Width / (FLOAT)Window->Bounds.Height, 1, 1000);                                                        // the far view-plane

	camera.SetProjection(&matProjection);

	camera.InitializeCameraPosition();
/*
	LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
	LARGE_INTEGER Frequency;

	QueryPerformanceFrequency(&Frequency);
	QueryPerformanceCounter(&StartingTime);

	//PreRenderUIComponents();

	QueryPerformanceCounter(&EndingTime);
	ElapsedMicroseconds.QuadPart = EndingTime.QuadPart - StartingTime.QuadPart;
	ElapsedMicroseconds.QuadPart *= 1000;
	float timeInMilliseconds = (float)ElapsedMicroseconds.QuadPart / Frequency.QuadPart;
	float timeInMicroseconds = timeInMilliseconds * 1000;
	float timeInNanoseconds = timeInMicroseconds * 1000;
	int x = 0;
*/
}

void Engine::PreRenderUIComponents()
{
	/*
	float color[4] = { 0.7f, 0.7f, 0.7f, 0.0f };
	context->OMSetRenderTargets(1, alt_rendertarget.GetAddressOf(), zbuffer.Get());
	context->ClearRenderTargetView(alt_rendertarget.Get(), color);
	context->ClearDepthStencilView(zbuffer.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	context->OMSetDepthStencilState(depthoffstate.Get(), 0);

	HLSLBuffer buffer;
	buffer.view_matrix = XMMatrixIdentity();
	context->UpdateSubresource(constantbuffer.Get(), 0, 0, &buffer, 0, 0);

	UINT stride = sizeof(VertexPositionTexture);
	UINT offset = 0;

	context->PSSetShaderResources(0, 1, font_texture.GetAddressOf());

	ComPtr<ID3D11Buffer> vbuffer;

	//Just written for testing purposes..
	int window_width = 775;
	int window_height = 454;

	int texture_width = 513 * 2;
	int texture_height = 235 * 2;

	float X = (float)texture_width / (float)window_width;
	float Y = (float)texture_height / (float)window_height;

	float  left = -1.0F;
	float right = -1.0f + X;
	float bottom = 1.0f - Y;
	float top = 1.0F;

	VertexPositionTexture square[6 * 100];

	float offsetPos = 0.1f;

	for (int j = 600; j > 0; j -= 6)
	{
		left += offsetPos;
		right += offsetPos;
		top -= offsetPos;
		bottom -= offsetPos;

		square[j - 6] = { left,  top, 0.0f,	0.0F, 0.0F }; //Top Left.
		square[j - 5] = { right, bottom, 0.0f,	1.0F, 1.0F }; //Bottom Right.
		square[j - 4] = { left, bottom, 0.0f,	0.0F, 1.0F }; //Bottom Left.

		square[j - 3] = { right, bottom, 0.0f,	1.0F, 1.0F }; //Bottom Right.
		square[j - 2] = { left,  top, 0.0f,	0.0F, 0.0F }; //Top Left.
		square[j - 1] = { right,  top, 0.0f,	1.0F, 0.0F }; //Top Right.
	}

	D3D11_BUFFER_DESC bd = { 0 };
	bd.ByteWidth = sizeof(VertexPositionTexture) * 600;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA srd = { square, 0, 0 };
	device->CreateBuffer(&bd, &srd, vbuffer.GetAddressOf());

	context->IASetVertexBuffers(0, 1, vbuffer.GetAddressOf(), &stride, &offset);

	float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->OMSetBlendState(blendstate.Get(), BlendFactor, 0xFFFFFFFF);

	context->Draw(600, 0);
	*/
}

void Engine::CreateGeometry()
{
	model_mesh.CreateCubeObject(device.Get(), 0.0F, 0.0F, 16.0F);
	model_mesh.CreateCubeObject(device.Get(), 0.0F, 0.0F, 12.0F);
	model_mesh.CreateCubeObject(device.Get(), 0.0F, 0.0F, 8.0F);

	font_resource.CreateGlyphMapping();
	font_resource.AddStringToBuffer(viewport, L"Hello World!\nTesting Line 2\n{Testing Line 3\n}Special Test", 0, 0);
	font_resource.PushStringsToDeviceBuffer(device.Get());
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


	HRESULT hr;
	hr = CreateWICTextureFromFile(device.Get(), nullptr, L"Assets/RGB_TransparencyMap.png", nullptr, &gradiant_texture, 0);
	hr = CreateWICTextureFromFile(device.Get(), nullptr, L"Assets/Wood.png", nullptr, &wood_texture, 0);
	hr = CreateWICTextureFromFile(device.Get(), nullptr, L"Assets/0_FONT.PNG", nullptr, &font_texture, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// this function performs updates to the state of the game
void Engine::Update()
{
	XGameInput::LoadController();
	XINPUT_GAMEPAD loaded_pad = XGameInput::GamePad();

	if (XGameInput::Compare(XINPUT_GAMEPAD_X))
	{
		camera.InitializeCameraPosition(); //Reset View Matrix.
		return;
	}

	if (XGameInput::Compare(XINPUT_GAMEPAD_A))
	{
		TEST_STATE = 1;
	}

	if (XGameInput::Compare(XINPUT_GAMEPAD_B))
	{
		TEST_STATE = 0;
	}

	signed short right_strength = loaded_pad.sThumbLX; //Move Left Right Joystick.
	signed short forward_strength = loaded_pad.sThumbLY; //Move Forward Back Joystick.

	signed short turn_strength = loaded_pad.sThumbRX; //Turn Left Right Joystick.
	signed short look_strength = loaded_pad.sThumbRY; //Look Up Down Joystick.

	unsigned int abs_forward = forward_strength * forward_strength;
	unsigned int abs_right = right_strength * right_strength;
	unsigned int abs_turn = turn_strength * turn_strength;
	unsigned int abs_look = look_strength * look_strength;

	unsigned int dead_zone =  6400 * 6400; //Should be about at least 20% of joystick needs to be pressed. 

	bool updated = false;

	//If the player did in fact turn updated the camera movement vectors. 
	if (abs_turn > dead_zone)
	{

		camera.rotation.y += turn_strength * 2;
		//camera.rotation.y = 32768 * 10;

		//0xB40000 = 360 * 32768 in hex form.
		if (camera.rotation.y < 0) 
			camera.rotation.y += 0xB40000;
		if (camera.rotation.y >= 0xB40000)
			camera.rotation.y -= 0xB40000;

		float value = camera.rotation.y / 32768.0F;
		double PI = 3.14159265;

		camera.forward.x = (float)sin(value * PI / 180);
		camera.forward.z = (float)cos(value * PI / 180);

		int var = (camera.rotation.y + 0x2D0000);
		if (var >= 0xB40000)
			var -= 0xB40000;
		value = var / 32768.0F;

		camera.right.x = (float)sin(value * PI / 180);  //Right Vector X without Up Vector.
		camera.right.z = (float)cos(value * PI / 180);  //Right Vector Z without Up Vector.

		updated = true;
	}

	if (abs_look > dead_zone)
	{
		camera.rotation.x -= look_strength * 2;
		//camera.rotation.x = 32768 * 10;

		//0x230000 = Hex version of 70.0.
		//We force the Look rotation to a range of -70 degrees and 70 degrees.
		if (camera.rotation.x < -0x230000)
			camera.rotation.x = -0x230000;
		if (camera.rotation.x > 0x230000)
			camera.rotation.x = 0x230000;

		signed int rotation = camera.rotation.x;
		double PI = 3.14159265;

		//Now we have a postive value between 0-70 (positive) or 290-359 (negative). 
		if (rotation < 0)
			rotation += 0xB40000;

		float value = rotation / 32768.0F;

		camera.right.y = (float)sin(value * PI / 180);
		camera.up.y = (float)cos(value * PI / 180);

		updated = true;
	}

	if (abs_forward > dead_zone)
	{
		float strength = (float)(forward_strength * 0.00003125 * 0.15);
		camera.position.x += strength * camera.forward.x;
		camera.position.z += strength * camera.forward.z;
		updated = true;
	}

	if (abs_right > dead_zone)
	{
		float strength = (float)(right_strength * 0.00003125 * 0.15);
		camera.position.x += strength * camera.right.x;
		camera.position.z += strength * camera.right.z;
		updated = true;
	}

	if (updated)
	{
		camera.UpdateViewMatrix();
	}
}

int j = 0;
int k = 0;

void Engine::BeginRender2D()
{
	context->OMSetBlendState(blendstate.Get(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(depthonstate.Get(), 0);
	context->RSSetState(rasterizerstate.Get());
	context->PSSetSamplers(0, 1, &sampler);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(inputlayout.Get());
	context->VSSetShader(vertexshader.Get(), nullptr, 0);
	context->PSSetShader(pixelshader.Get(), nullptr, 0);
	context->VSSetConstantBuffers(0, 1, constantbuffer.GetAddressOf());

	//HLSLBuffer buffer;
}

void Engine::EndRender2D()
{
		
}

void Engine::Render()
{
	//Background Color.
	float color[4] = { 0.7f, 0.7f, 0.7f, 0.0f };

	//Clear Buffers Information So We Can Start Over :) Otherwise Nothing Gets Rendered :(

	context->OMSetRenderTargets(1, rendertarget.GetAddressOf(), zbuffer.Get());
	context->ClearRenderTargetView(rendertarget.Get(), color);

	context->ClearDepthStencilView(zbuffer.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	//3D RENDER -->

	//Set Texture to GradiantTexture For Testing.. 
	context->PSSetShaderResources(0, 1, gradiant_texture.GetAddressOf());

	//Set the players camera view matrix.
	HLSLBuffer buffer;

	buffer.view_matrix = XMLoadFloat4x4(&camera.view_matrix) * XMLoadFloat4x4(&camera.projection);
	context->UpdateSubresource(constantbuffer.Get(), 0, 0, &buffer, 0, 0);

	//Not Needed But We Store Because Why Not...
	UINT stride = sizeof(VertexPositionTexture);
	UINT offset = 0;

	//Set Cube Vertex/Index Buffer Data.
	context->IASetVertexBuffers(0, 1, model_mesh.vertexbuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(model_mesh.indexbuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	//Draw 3 Cubes.
	context->DrawIndexed(36 * 3, 0, 0);
	
	//2D RENDER -->

	//Set Camera to Identity so it draws in 2D Coordinates on screen.

	//Set Texture To Font Texture.
	//context->PSSetShaderResources(0, 1, font_texture.GetAddressOf());

	//Set Vertex Data to Font Vertex Buffer Data

	buffer.view_matrix = XMMatrixIdentity();
	context->UpdateSubresource(constantbuffer.Get(), 0, 0, &buffer, 0, 0);
	
	//Turn On Alpha Blending.
	float BlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->OMSetBlendState(blendstate.Get(), BlendFactor, 0xFFFFFFFF);
	
	if (TEST_STATE == 1)
	{
		context->PSSetShaderResources(0, 1, font_texture.GetAddressOf());
		context->IASetVertexBuffers(0, 1, font_resource.buffer.GetAddressOf(), &stride, &offset);
		context->Draw(font_resource.GetSize(), 0);
	}

	//model_mesh.DrawString(context.Get(), L"Hello World!", 50, 50);

	//Turn Off Alpha Blending.
	context->OMSetBlendState(NULL, NULL, 0xFFFFFFFF); //Turn OFF Blending.

	//Present Swap Chain..

	swapchain->Present(1, 0);
}