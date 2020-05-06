#pragma once

#ifndef SFILEIO_H
#define SFILEIO_H

#include <Windows.h>
#include <string>

namespace ns_HoLin
{
	struct sFileIO
	{
		HANDLE hfile = NULL;
		char ch = ' ';
		DWORD page_size_in_bytes = 0;
		char *file_buffer = NULL;
		DWORD bytes_read_from_file = 0;
		DWORD index_of_next_char_to_read = 0;
		std::string prevpage;

		sFileIO(const wchar_t*);
		~sFileIO();
		BOOL GetNextCharFromBuffer(BOOL);
	};
}

#endif
