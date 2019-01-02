#pragma once

#include "FileReaderWriter.h"
#include "CameraEngine.h"
#include "VertexStructureTypes.h"
#include "XGameInput.h"
#include "XModelMesh.h"
#include "ContentLoader.h"
#include "GameTime.h"

using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace DirectX;

class Engine
{

	struct HLSLBuffer
	{
		XMFLOAT4X4 view_matrix;
	};

	public:
		static ComPtr<ID3D11Device1> device;			// the device interface
		static ComPtr<ID3D11DeviceContext1> context;	// the device context interface
		static D3D11_VIEWPORT viewport;

		ComPtr<IDXGISwapChain1> swapchain;      // the swap chain interface
		ComPtr<ID3D11RenderTargetView> rendertarget;

		ComPtr<ID3D11VertexShader> vertexshader;
		ComPtr<ID3D11PixelShader> pixelshader;
		ComPtr<ID3D11InputLayout> inputlayout;

		ComPtr<ID3D11Buffer> d3d_const_buffer;
		ComPtr<ID3D11Buffer> d2d_const_buffer;

		ComPtr<ID3D11Buffer> vertexbuffer;

		ComPtr<ID3D11DepthStencilView> zbuffer;

		ComPtr<ID3D11DepthStencilState> depthOffState;
		ComPtr<ID3D11DepthStencilState> depthOnState;

		ComPtr<ID3D11RasterizerState> rasterizerstate;

		ComPtr<ID3D11ShaderResourceView> wood_texture;
		ComPtr<ID3D11ShaderResourceView> font_texture;
		ComPtr<ID3D11ShaderResourceView> gradiant_texture;
		ComPtr<ID3D11ShaderResourceView> test_texture;

		ComPtr<ID3D11BlendState> blendstate;

		ComPtr<ID3D11DepthStencilState> depthonstate;
		ComPtr<ID3D11DepthStencilState> depthoffstate;
		ComPtr<ID3D11SamplerState> sampler;

		ComPtr<ID3D11Texture2D> zbuffertexture2;
		ComPtr<ID3D11RenderTargetView> alt_rendertarget;
		ComPtr<ID3D11ShaderResourceView> alt_shadertexture;

		//static ComPtr<ID3D11ShaderResourceView*> __textures_alias;

	private:
		FileReaderWriter file_reader;
		CameraEngine camera;
		FontResource font_resource;
		XModelMesh model_mesh;
		uint16 controller_input;

	public:
		Engine();
		~Engine();
		void Initialize();
		void CreatePipeline();
		void CreateDynamicBuffer();
		void Update();

		void Render();
};