#pragma once

#include <string>

class FileReaderWriter
{
	public:
		Platform::Array<byte>^ ReadBytesFromFile(std::string File);
		Platform::Array<byte>^ ReadImageFromFile(std::string File);
};