#include "_PreHeader.hpp"
#include "HashFuncs.hpp"

using namespace StdLib;

// Reverses (reflects) bits in a 32-bit word.
static inline ui32 reverse(ui32 x) 
{
	x = ((x & 0x55555555) << 1) | ((x >> 1) & 0x55555555);
	x = ((x & 0x33333333) << 2) | ((x >> 2) & 0x33333333);
	x = ((x & 0x0F0F0F0F) << 4) | ((x >> 4) & 0x0F0F0F0F);
	x = (x << 24) | ((x & 0xFF00) << 8) | ((x >> 8) & 0xFF00) | (x >> 24);
	return x;
}

// based on https://www.hackersdelight.org/hdcodetxt/crc.c.txt
// the most basic implementation
ui32 Hash::CRC32(const ui8 *message)
{
	i32 crc = 0xFFFFFFFF;
	for (uiw i = 0; message[i]; ++i)
	{
		ui32 byte = message[i];
		byte = reverse(byte);
		for (uiw j = 0; j <= 7; j++) 
		{
			if (static_cast<i32>(crc ^ byte) < 0)
			{
				crc = (crc << 1) ^ 0x04C11DB7;
			}
			else
			{
				crc = crc << 1;
			}
			byte = byte << 1;
		}
	}
	return reverse(static_cast<ui32>(~crc));
}

ui32 Hash::CRC32(const ui8 *message, uiw length)
{
	i32 crc = 0xFFFFFFFF;
	for (uiw i = 0; i < length; ++i)
	{
		ui32 byte = message[i];
		byte = reverse(byte);
		for (uiw j = 0; j <= 7; j++)
		{
			if (static_cast<i32>(crc ^ byte) < 0)
			{
				crc = (crc << 1) ^ 0x04C11DB7;
			}
			else
			{
				crc = crc << 1;
			}
			byte = byte << 1;
		}
	}
	return reverse(static_cast<ui32>(~crc));
}