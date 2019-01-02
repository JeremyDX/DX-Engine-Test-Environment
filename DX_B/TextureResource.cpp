#include "pch.h"
#include "TextureResource.h"

TextureResource::TextureResource()
{

}

TextureResource::~TextureResource()
{
	if(m_texture.Get() != NULL)
		m_texture.Get()->Release();
}
