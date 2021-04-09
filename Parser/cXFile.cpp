#include "stdafx.h"

namespace ns_HoLin
{
	cXFile::cXFile()
	{
		file_type = TEXT_FILE;
		floatsize = 32;
		hfile = nullptr;
	}

	cXFile::~cXFile()
	{
		Cleanup();
	}

	void cXFile::Cleanup()
	{
		file_type = TEXT_FILE;
		floatsize = 32;
		if (hfile) {
			CloseHandle(hfile);
			hfile = nullptr;
		}
		text.xfiledata.Cleanup();
	}
	
	BOOL cXFile::ReadCommandLineArgumentsThenParse(DWORD argv, const wchar_t **argc)
	{
		if (hfile)
			return FALSE;
		Cleanup();
		text.xfiledata.Cleanup();

		std::array<std::wstring, 2> state_strings{ // Do not reposition strings
			std::wstring(L"-trackoutput"),
			std::wstring(L"-f"),
		};
		std::unordered_map<std::wstring, DWORD> map_options{
			std::make_pair(state_strings[0], 0),
			std::make_pair(state_strings[1], 1)
		};
		std::array<std::any, state_strings.size()> arguments_options;
		BOOL btrack = FALSE;

		for (DWORD i = 1; i < argv; ++i) {
			if (state_strings[0] == std::wstring(argc[i])) {
				arguments_options[map_options[state_strings[0]]] = TRUE;
			}
			else if (state_strings[1] == std::wstring(argc[i])) {
				if (argv <= (i + 1)) {
#if defined(_CONSOLE)
					std::wcout << L"Error -f option, no file name entered, exiting.\n";
#endif
					return FALSE;
				}
				arguments_options[map_options[state_strings[1]]] = std::wstring((const wchar_t*)argc[++i]);
			}
			else {
#if defined(_CONSOLE)
				std::wcout << L"Quitting unknown command \'" << (const wchar_t*)argc[i] << L"\'\n";
#endif
				return FALSE;
			}
		}
		if (arguments_options[map_options[state_strings[1]]].has_value()) {
			if (!this->openfile(std::any_cast<std::wstring>(arguments_options[map_options[state_strings[1]]]).c_str()))
				return FALSE;
		}
		if (!hfile) {
			OpenFileWithMeshFileName();
		}
		if (hfile) {
#if defined(_CONSOLE)
			std::wcout << L"File opened.\n";
#endif
			try {
				if (arguments_options[map_options[state_strings[0]]].has_value())
					btrack = std::any_cast<BOOL>(arguments_options[map_options[state_strings[0]]]);
				if (this->ParseFile(btrack)) {
#if defined(_CONSOLE)
					std::wcout << L"File parsed successfully.\n";
#endif
					CloseHandle(hfile);
					hfile = nullptr;
					return TRUE;
				}
				else {
#if defined(_CONSOLE)
					std::wcout << L"Error parsing file.\n";
#endif
				}
			}
			catch (ns_HoLin::sErrorMessageException serror) {
				std::cerr << serror.what();
			}
			catch (std::exception e) {
				std::cerr << e.what() << '\n';
			}
			CloseHandle(hfile);
			hfile = nullptr;
		}
		return FALSE;
	}

	BOOL cXFile::ReadXFile(const wchar_t *file_name)
	{
		BOOL success = FALSE;
		
		if (hfile)
			return success;
		Cleanup();
		text.xfiledata.Cleanup();
		if (file_name) {
			if (openfile(file_name)) {
				success = this->ParseFile(FALSE);
				CloseHandle(hfile);
				hfile = nullptr;
			}
			else {
#ifdef _WINDOWS
				MessageBox(nullptr, L"Could not open mesh file.", L"Error", MB_OK);
#else
				std::wcout << L"Could not open mesh file.\n";
#endif
			}
		}
		else {
#ifdef _WINDOWS
			MessageBox(nullptr, L"No file name entered.", L"Error", MB_OK);
#else
			std::wcout << L"Error no file name entered.\n";
#endif
		}
		return success;
	}

	void cXFile::OpenFileWithMeshFileName()
	{
		std::wifstream fin;
		wchar_t buff[MAX_PATH];
		DWORD bytes_read = 0;

		buff[0] = L'\0';
		fin.open(L"file.txt");
		if (fin.is_open()) {
			fin.getline(buff, MAX_PATH);
			if (std::wcslen(buff) > 0) {
				std::wcout << buff << L'\n';
				openfile(buff);
			}
			else {
				std::wcout << L"\nError \'file.txt\' empty.\n";
			}
			fin.close();
		}
		else {
			std::wcout << L"\nError opening \'file.txt\'.\n";
		}
	}

	bool cXFile::openfile(const wchar_t *file_name)
	{
		hfile = CreateFile(file_name, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hfile != INVALID_HANDLE_VALUE)
			return true;
#if defined(_CONSOLE)
		std::wcout << L"Error could not locate " << file_name << L'\n';
#endif
#if defined(_WINDOWS)
		std::wstring s(L"Error could not locate ");
		s.append(file_name);
		MessageBox(nullptr, s.c_str(), L"File error", MB_OK);
#endif
		hfile = nullptr;
		return false;
	}

	BOOL cXFile::ReadHeader()
	{
		struct sHeader {
			char xfileindicator[4];
			char version[4];
			char format[4];
			char floatsize[4];
		};

		sHeader headerbuffer;
		DWORD bytesread = 0;
		long data = 0;

		if (ReadFile(hfile, (LPVOID)&headerbuffer, sizeof(headerbuffer), &bytesread, nullptr)) {
			if (bytesread == sizeof(headerbuffer)) {
				if (memcpy_s((void*)&data, sizeof(long), (const void*)&headerbuffer.xfileindicator, sizeof(char) * 4) == 0) {
					switch (data) {
					case XOFFILE_FORMAT_MAGIC:
						break;
					default:
#if defined(_WINDOWS)
						MessageBox(nullptr, L"Error not an x file.", L"Error", MB_OK);
#elif defined(_CONSOLE)
						std::wcout << L"Error not an x file.\n";
#endif
						return FALSE;
					}
				}
				if (memcpy_s((void*)&data, sizeof(long), (const void*)&headerbuffer.version, sizeof(char) * 4) == 0) {
					switch (data) {
					case XOFFILE_FORMAT_VERSION:
						break;
					default:
#if defined(_WINDOWS)
						MessageBox(nullptr, L"Error unknown version.\r\nOnly version 0303 supported.", L"Error", MB_OK);
#elif defined(_CONSOLE)
						std::wcout << L"Error unknown version.\nOnly version 0303 supported.\n";
#endif
						return FALSE;
					}
				}
				if (memcpy_s((void*)&data, sizeof(long), (const void*)&headerbuffer.format, sizeof(char) * 4) == 0) {
					switch (data) {
					case XOFFILE_FORMAT_TEXT:
						file_type = TEXT_FILE;
						break;
					case XOFFILE_FORMAT_BINARY:
						file_type = BINARY_FILE;
#if defined(_WINDOWS)
						MessageBox(nullptr, L"Binary version not supported at this time.", L"Error", MB_OK);
#elif defined(_CONSOLE)
						std::wcout << L"Binary version not supported at this time.\n";
#endif
						return FALSE;
					case XOFFILE_FORMAT_COMPRESSED:
						file_type = ZIP_FILE;
#if defined(_WINDOWS)
						MessageBox(nullptr, L"Can't read compressed file.", L"Error", MB_OK);
#elif defined(_CONSOLE)
						std::wcout << L"Can't read compressed file.\n";
#endif
						return FALSE;
					default:
#if defined(_WINDOWS)
						MessageBox(nullptr, L"Header error, unknown file format.", L"Error", MB_OK);
#elif defined(_CONSOLE)
						std::wcout << L"Header error, unknown file format.\n";
#endif
						return FALSE;
					}
				}
				if (memcpy_s((void*)&data, sizeof(long), (const void*)&headerbuffer.floatsize, sizeof(char) * 4) == 0) {
					switch (data) {
					case XOFFILE_FORMAT_FLOAT_BITS_32:
						floatsize = 32;
						break;
					case XOFFILE_FORMAT_FLOAT_BITS_64:
						floatsize = 64;
#if defined(_WINDOWS)
						MessageBox(nullptr, L"64 bit float not supported at this time.", L"Error", MB_OK);
#elif defined(_CONSOLE)
						std::wcout << L"64 bit float not supported at this time.\n";
#endif
						return FALSE;
					default:
#if defined(_WINDOWS)
						MessageBox(nullptr, L"Unknown float size.", L"Error", MB_OK);
#elif defined(_CONSOLE)
						std::wcout << L"Unknown float size.\n";
#endif
						return FALSE;
					}
				}
			}
			else {
#if defined(_WINDOWS)
				MessageBox(nullptr, L"Unable to read X file header.", L"Error", MB_OK);
#elif defined(_CONSOLE)
				std::wcout << L"Unable to read X file header.\n";
#endif
				return FALSE;
			}
		}
		return TRUE;
	}

	BOOL cXFile::ParseFile(BOOL btrack)
	{
		if (ReadHeader()) {
			if (file_type == TEXT_FILE) {
				return text.ParseFile(&hfile, btrack);
			}
		}
		return FALSE;
	}
}
