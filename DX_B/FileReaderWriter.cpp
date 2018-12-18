#include "pch.h"
#include "FileReaderWriter.h"

Platform::Array<byte>^ FileReaderWriter::ReadBytesFromFile(std::string File)
{
	Platform::Array<byte>^ data = nullptr;

	// open the file
	std::ifstream file(File, std::ios::in | std::ios::binary | std::ios::ate);

	// if open was successful
	if (file.is_open())
	{
		// find the length of the file
		int Length = (int)file.tellg();

		// collect the file data
		data = ref new Platform::Array<byte>(Length);
		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char*>(data->Data), Length);
		file.close();
	}

	return data;
}

Platform::Array<byte>^ FileReaderWriter::ReadImageFromFile(std::string File)
{
	return nullptr;
}
