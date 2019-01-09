#pragma once

#include "FileReaderWriter.h"

class TextureResource
{
	public:
		TextureResource();
		~TextureResource();
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
};