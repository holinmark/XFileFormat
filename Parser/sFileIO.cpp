#include "stdafx.h"

namespace ns_HoLin
{
	cFileInput::cFileInput()
	{
		SYSTEM_INFO sys_info;
		
		hfile = nullptr;
		page_size_in_bytes = 0;
		file_buffer = NULL;
		bytes_read_from_file = 0;
		index_of_next_char_to_read = 0;
		page_size_in_bytes = 0;
		GetSystemInfo(&sys_info);
		if (sys_info.dwPageSize > 0) {
			page_size_in_bytes = sys_info.dwPageSize * 10;
			file_buffer = new char[page_size_in_bytes];
		}
		ch = ' ';
		endoffile = FALSE;
	}

	cFileInput::~cFileInput()
	{
		hfile = nullptr;
		page_size_in_bytes = 0;
		bytes_read_from_file = 0;
		index_of_next_char_to_read = 0;
		page_size_in_bytes = 0;
		DeleteBuffer();
		ch = ' ';
		endoffile = FALSE;
	}

	cFileInput::operator bool() const
	{
		return (bool)(*hfile != nullptr);
	}

	BOOL cFileInput::MoveBufferIndex(BOOL btrackoutput)
	{
		if (index_of_next_char_to_read == bytes_read_from_file) {
			prevpage = file_buffer;
			index_of_next_char_to_read = 0;
			file_buffer[0] = '\0';
			if (!ReadFile(*hfile, (LPVOID)file_buffer, (DWORD)(page_size_in_bytes - 1), (LPDWORD)&bytes_read_from_file, nullptr)) {
				hfile = nullptr;
				return FALSE;
			}
			if (bytes_read_from_file == 0) {
				hfile = nullptr;
				endoffile = TRUE;
				return FALSE;
			}
			file_buffer[bytes_read_from_file] = '\0';
		}
		ch = file_buffer[index_of_next_char_to_read++];
		return TRUE;
	}

	BOOL cFileInput::GetBytesFromFile(char *buffer, std::size_t blen, std::size_t &bytes_to_read)
	{
		if (!ReadFile(*hfile, (LPVOID)buffer, (DWORD)bytes_to_read, (LPDWORD)&bytes_read_from_file, nullptr)) {
			hfile = nullptr;
			return FALSE;
		}
		if (bytes_read_from_file == 0) {
			hfile = nullptr;
			return FALSE;
		}
		return TRUE;
	}

	void cFileInput::GetOffendingLine(std::string &before, std::string &after)
	{
		DWORD index = 0, count = 0;

		for (index = index_of_next_char_to_read; index < GetPageSize(); ++index) {
			if (file_buffer[index] == '\n')
				break;
			else {
				after.append(1, file_buffer[index]);
			}
		}
		for (index = index_of_next_char_to_read; index < GetPageSize(); --index) {
			if (index == 0) {
				before.insert(std::begin(before), file_buffer[index]);
				for (count = (DWORD)(prevpage.size() - 1); count < GetPageSize(); --count) {
					if (prevpage[count] == '\n')
						break;
					before.insert(std::begin(before), prevpage[count]);
				}
				break;
			}
			before.insert(std::begin(before), file_buffer[index]);
			if (file_buffer[index] == '\n')
				break;
		}
	}

	void cFileInput::DeleteBuffer()
	{
		if (file_buffer) {
			delete[] file_buffer;
			file_buffer = nullptr;
		}
	}
}
