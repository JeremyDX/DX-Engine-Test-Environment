#include "pch.h"
#include "TextureResource.h"


ID3D11ShaderResourceView* TextureResource::GetTexture()
{
	return nullptr;
}

HRESULT TextureResource::CreateFromFile(ID3D11Device1* device, LPCSTR fileName)
{
	return CreateWICTextureFromFile(device, nullptr, L"Wood.png", nullptr, &m_texture, 0);
}
