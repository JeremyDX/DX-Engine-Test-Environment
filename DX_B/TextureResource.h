#pragma once

#include "pch.h"
#include "FileReaderWriter.h"

using namespace DirectX;

class TextureResource
{
	public:
		TextureResource();
		~TextureResource();
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
};