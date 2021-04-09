#pragma once

#ifndef SFILEIO_H
#define SFILEIO_H

#include <Windows.h>
#include <string>

namespace ns_HoLin
{
	class cFileInput
	{
		PHANDLE hfile = nullptr;
		DWORD page_size_in_bytes = 0;
		char *file_buffer = NULL;
		DWORD bytes_read_from_file = 0;
		DWORD index_of_next_char_to_read = 0;
		std::string prevpage;
		char ch;
		BOOL endoffile;

	public:
		cFileInput();
		~cFileInput();
		explicit operator bool() const;
		BOOL MoveBufferIndex(BOOL);
		char GetNextCharToProcess() { return ch; }
		BOOL GetBytesFromFile(char*, std::size_t, std::size_t&);
		void DeleteBuffer();
		BOOL GetEndOfFileStatus() { return endoffile; }
		void SetEndOfFileStatus(BOOL b) { endoffile = b; }
		void SetFileHandle(PHANDLE pfile) { hfile = pfile; }
		DWORD GetPageSize() { return page_size_in_bytes; }
		void GetOffendingLine(std::string&, std::string&);
	};
}

#endif
