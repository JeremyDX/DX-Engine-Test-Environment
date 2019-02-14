#pragma once

#include <string>

struct FileReaderWriter
{
	static Platform::Array<byte>^ ReadBytesFromFile(std::string File);
	static void ReadImageFromFile(LPCWSTR str);
};
