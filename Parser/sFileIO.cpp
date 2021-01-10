#include "stdafx.h"

namespace ns_HoLin
{
	sFileIO::sFileIO(const wchar_t *filename)
	{
		SYSTEM_INFO sys_info;
		
		page_size_in_bytes = 0;
		file_buffer = NULL;
		bytes_read_from_file = 0;
		index_of_next_char_to_read = 0;
		page_size_in_bytes = 0;
		GetSystemInfo(&sys_info);
		if (sys_info.dwPageSize > 0) {
			page_size_in_bytes = sys_info.dwPageSize * 10;
			file_buffer = new char[page_size_in_bytes];
			if (file_buffer) {
				hfile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hfile == INVALID_HANDLE_VALUE)
					hfile = nullptr;
			}
		}
	}

	sFileIO::~sFileIO()
	{
		if (file_buffer) {
			delete[] file_buffer;
			file_buffer = nullptr;
		}
		Close();
	}
	
	void sFileIO::Close()
	{
		if (hfile) {
			CloseHandle(hfile);
			hfile = nullptr;
		}
	}

	BOOL sFileIO::GetNextCharFromBuffer(BOOL btrackoutput)
	{
		if (index_of_next_char_to_read == bytes_read_from_file) {
			if (index_of_next_char_to_read > 0) {
				if (btrackoutput) {
					std::cout << file_buffer;
				}
			}
			prevpage = file_buffer;
			index_of_next_char_to_read = 0;
			file_buffer[0] = '\0';
			if (ReadFile(hfile, (LPVOID)file_buffer, (DWORD)(page_size_in_bytes - 1), (LPDWORD)&bytes_read_from_file, nullptr) == FALSE)
				return FALSE;
			if (bytes_read_from_file == 0)
				return FALSE;
			file_buffer[bytes_read_from_file] = '\0';
		}
		ch = file_buffer[index_of_next_char_to_read++];
		return TRUE;
	}
}