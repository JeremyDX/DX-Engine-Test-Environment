#pragma once

#include "pch.h"
#include "FileReaderWriter.h"

using namespace DirectX;

class TextureResource
{
	public:
		HRESULT CreateFromFile(ID3D11Device1* device, LPCSTR fileName);
		ID3D11ShaderResourceView* GetTexture();

	private:

		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
};