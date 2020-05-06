#pragma once

#include "cTextXFileParser.h"
#include "cBinaryXFileParser.h"

namespace ns_HoLin
{
	struct sXFile
	{
	private:
		BOOL success;
	public:
		BOOL textfile, mode32bit;
		//ns_HoLin::cTextXFileParser ctext;
		//ns_HoLin::cBinaryXFileParser cbinary;

		sXFile(const wchar_t*);
		~sXFile();
		BOOL GetXFileHeader(const wchar_t*);
		BOOL ParseFile();
	};
}
