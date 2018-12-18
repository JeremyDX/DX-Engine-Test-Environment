#pragma once

#include <fstream>
#include <string>

#include "WICTextureLoader.h"

class FileReaderWriter
{
	public:
		Platform::Array<byte>^ ReadBytesFromFile(std::string File);
		Platform::Array<byte>^ ReadImageFromFile(std::string File);
};