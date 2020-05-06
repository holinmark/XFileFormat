#pragma once

#ifndef  CBINARYXFILEPARSER_H
#define CBINARYXFILEPARSER_H

#include "sFileIO.h"

namespace ns_HoLin
{
	class cBinaryXFileParser
	{
	public:
		ns_HoLin::sFileIO sfile;
		
		cBinaryXFileParser(const wchar_t*);
		~cBinaryXFileParser();
		BOOL ParseFile();
	};
}

#endif
