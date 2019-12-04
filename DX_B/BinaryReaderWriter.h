#pragma once

struct BinaryReaderWriter
{
	static void ReadBinaryData(const char* name, char* buffer_reference, int& length);

	static void AddShadersToCache();

	static  int8_t ReadByte();
	static int16_t ReadShort();
	static int32_t ReadInt();
	static int64_t ReadLong();

	static void SkipBytes(int length);
	static  int SkipAndTellBytes(int length);

	static void WriteIndexSizeByte(int seek, int8_t value);
	static void WriteIndexSizeShort(int seek, int value);
	static void WriteIndexSizeInt(int seek, int value);

	static void WriteByte(int8_t value);
	static void WriteByte(int8_t value, int position);

	static void WriteShort(int16_t value);
	static void WriteShort(int16_t value, int position);

	static void WriteInt(int32_t value);
	static void WriteInt(int32_t value, int position);

	static void WriteLong(int64_t value);

	static void WriteLEShort(int16_t value);
	static void WriteLEInt(int32_t value);
	static void WriteLELong(int64_t value);
};