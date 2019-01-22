#include "pch.h"

#include <fstream>
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

void FileReaderWriter::ReadImageFromFile(LPCWSTR str)
{
	/*IWICImagingFactory* pWIC = _GetWIC();
	IWICBitmapDecoder *decoder;
	IWICBitmapFrameDecode *frame;
	HRESULT hr = pWIC->CreateDecoderFromFilename(str, 0, GENERIC_READ, WICDecodeMetadataCacheOnDemand, &decoder);
	decoder->GetFrame(0, &frame);
	UINT width, height;
	hr = frame->GetSize(&width, &height);
	WICPixelFormatGUID pixelFormat;
	hr = frame->GetPixelFormat(&pixelFormat);
	WICPixelFormatGUID convertGUID;
	memcpy(&convertGUID, &pixelFormat, sizeof(WICPixelFormatGUID));
	DXGI_FORMAT format = _WICToDXGI(pixelFormat)*/
}
