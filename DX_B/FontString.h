#pragma once

struct FontString
{
	private:
		unsigned __int32 packedstringdata;

		//FontID			  =  5 Bits (Fonts 0 - 31).
		//TextSize			  =  7 Bits (Length 0 - 127).
		//VertexPosition	  = 10 Bits (Length 0 - 1023).
		//UNUSED_DATA		  =  9 Bits (Length 0 - 511 Remainder).
		//PackedSize		  = 22 Bits (Total Packed Data Size).
};