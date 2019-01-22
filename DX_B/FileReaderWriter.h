#pragma once

#include <string>

class FileReaderWriter
{
	public:
		Platform::Array<byte>^ ReadBytesFromFile(std::string File);
		static void ReadImageFromFile(LPCWSTR str);
};
