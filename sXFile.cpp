#include "stdafx.h"

namespace ns_HoLin
{
	sXFile::sXFile(const wchar_t *file_name)
	{
		textfile = FALSE;
		mode32bit = FALSE;
		success = FALSE;
		success = GetXFileHeader(file_name);
	}

	sXFile::~sXFile()
	{
	}

	BOOL sXFile::GetXFileHeader(const wchar_t *file_name)
	{
		struct sXFileHeader
		{
			char magic_number[4];
			char major[2];
			char minor[2];
			char format_type[4];
			char float_size[4];
			char pad;
		};
		HANDLE hfile = NULL;
		sXFileHeader sxheader;
		DWORD datasize = sizeof(sXFileHeader) - sizeof(char);
		DWORD bytesreceived = 0;

		hfile = CreateFile(file_name, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hfile == INVALID_HANDLE_VALUE) {
			hfile = nullptr;
			return FALSE;
		}
		memset((void*)&sxheader, 0, sizeof(sXFileHeader));
		if (ReadFile(hfile, (LPVOID)&sxheader, datasize, (LPDWORD)&bytesreceived, nullptr) == FALSE) {
			CloseHandle(hfile);
			hfile = nullptr;
			return FALSE;
		}
		CloseHandle(hfile);
		hfile = nullptr;
		if (bytesreceived != sizeof(sXFileHeader)) {
			std::clog << "Could not read the header, " << sizeof(sXFileHeader) << " bytes from file.\n";
			return FALSE;
		}
		if (strstr((const char*)sxheader.magic_number, (const char*)XOFFILE_FORMAT_MAGIC) == NULL) {
			std::cout << "Not an x file format file.\n";
			return FALSE;
		}
		if (strstr((const char*)sxheader.format_type, (const char*)XOFFILE_FORMAT_TEXT) != NULL) {
			textfile = TRUE;
			std::cout << "Text file.\n";
		}
		else if (strstr((const char*)sxheader.format_type, (const char*)XOFFILE_FORMAT_BINARY) != NULL) {
			textfile = FALSE;
			std::cout << "Binary file.\n";
		}
		if (strstr((const char*)sxheader.float_size, (const char*)XOFFILE_FORMAT_FLOAT_BITS_32) != NULL) {
			mode32bit = TRUE;
			std::cout << "32 bit floating point.\n";
		}
		else if (strstr((const char*)sxheader.float_size, (const char*)XOFFILE_FORMAT_FLOAT_BITS_64) != NULL) {
			mode32bit = FALSE;
			std::cout << "64 bit floating point.\n";
		}
		return TRUE;
	}

	BOOL sXFile::ParseFile()
	{
		return FALSE;
	}
}
