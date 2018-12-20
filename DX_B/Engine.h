#pragma once

#include "FileReaderWriter.h"
#include "CameraEngine.h"
#include "VertexStructureTypes.h"
#include "XGameInput.h"
#include "XModelMesh.h"


using namespace Microsoft::WRL;
using namespace Windows::UI::Core;
using namespace DirectX;

class Engine
{

	struct HLSLBuffer
	{
		XMMATRIX view_matrix;
	};

	public:
		ComPtr<ID3D11Device1> device;			// the device interface
		ComPtr<ID3D11DeviceContext1> context;	// the device context interface

		ComPtr<IDXGISwapChain1> swapchain;      // the swap chain interface
		ComPtr<ID3D11RenderTargetView> rendertarget;

		ComPtr<ID3D11VertexShader> vertexshader;
		ComPtr<ID3D11PixelShader> pixelshader;
		ComPtr<ID3D11InputLayout> inputlayout;
		ComPtr<ID3D11Buffer> constantbuffer;

		ComPtr<ID3D11Buffer> vertexbuffer;

		ComPtr<ID3D11DepthStencilView> zbuffer;
		ComPtr<ID3D11DepthStencilState> depthOffState;
		ComPtr<ID3D11DepthStencilState> depthOnState;

		ComPtr<ID3D11RasterizerState> rasterizerstate;

		ComPtr<ID3D11ShaderResourceView> wood_texture;
		ComPtr<ID3D11ShaderResourceView> font_texture;

		ComPtr<ID3D11BlendState> blendstate;                      // the blend state interface


	private:
		FileReaderWriter file_reader;
		CameraEngine camera;
		XModelMesh model_mesh;
		float time;
		int camX, camY;
		uint16 controller_input;

	public:
		Engine();
		~Engine();
		void Initialize();
		void CreateGeometry();
		void CreatePipeline();
		void Update();

		void Render();
};