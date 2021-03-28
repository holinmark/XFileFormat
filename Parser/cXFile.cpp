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
					std::cerr << "Error -f option, no file name entered, exiting.\n";
					return FALSE;
				}
				arguments_options[map_options[state_strings[1]]] = std::wstring(argc[++i]);
			}
			else {
				std::wcerr << L"Quitting unknown command \'" << (const wchar_t*)argc[i] << L"\'\n";
				return FALSE;
			}
		}
		if (arguments_options[map_options[state_strings[1]]].has_value()) {
			this->openfile(std::any_cast<std::wstring>(arguments_options[map_options[state_strings[1]]]).c_str());
		}
		if (!hfile) {
			OpenFileWithMeshFileName();
		}
		if (hfile) {
			std::wcout << "File opened.\n";
			try {
				if (arguments_options[map_options[state_strings[0]]].has_value())
					btrack = std::any_cast<BOOL>(arguments_options[map_options[state_strings[0]]]);
				if (this->ParseFile(btrack)) {
					std::clog << "File parsed successfully.\n";
					CloseHandle(hfile);
					hfile = nullptr;
					return TRUE;
				}
				else {
					std::clog << "Error parsing file.\n";
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
		std::clog << "Error.\n";
		return FALSE;
	}

	BOOL cXFile::ReadXFile(const wchar_t *file_name)
	{
		if (hfile)
			return FALSE;
		Cleanup();
		text.xfiledata.Cleanup();
		if (file_name) {
			openfile(file_name);
			if (hfile) {
				BOOL r = this->ParseFile(FALSE);
				CloseHandle(hfile);
				hfile = nullptr;
				return r;
			}
		}
		return FALSE;
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
				std::cerr << "\nError \'file.txt\' empty.\n";
			}
			fin.close();
		}
		else {
			std::cerr << "\nError opening \'file.txt\'.\n";
		}
	}

	bool cXFile::openfile(const wchar_t *file_name)
	{
		hfile = CreateFile(file_name, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (hfile != INVALID_HANDLE_VALUE)
			return true;
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
#ifdef _WINDOWS
						MessageBox(nullptr, L"Error not an x file.", L"Error", MB_OK);
#else
						std::cout << "Error not an x file.\n";
#endif
						return FALSE;
					}
				}
				if (memcpy_s((void*)&data, sizeof(long), (const void*)&headerbuffer.version, sizeof(char) * 4) == 0) {
					switch (data) {
					case XOFFILE_FORMAT_VERSION:
						break;
					default:
#ifdef _WINDOWS
						MessageBox(nullptr, L"Error unknown version.", L"Error", MB_OK);
#else
						std::cout << "Error unknown version.\n";
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
#ifdef _WINDOWS
						MessageBox(nullptr, L"Binary version not supported at this time.", L"Error", MB_OK);
#else
						std::cout << "Binary version not supported at this time.\n";
#endif
						return FALSE;
					case XOFFILE_FORMAT_COMPRESSED:
						file_type = ZIP_FILE;
#ifdef _WINDOWS
						MessageBox(nullptr, L"Can't read compressed file.", L"Error", MB_OK);
#else
						std::cout << "Can't read compressed file.\n";
#endif
						return FALSE;
					default:
#ifdef _WINDOWS
						MessageBox(nullptr, L"Error unknown file format.", L"Error", MB_OK);
#else
						std::cout << "Error unknown file format.\n";
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
#ifdef _WINDOWS
						MessageBox(nullptr, L"64 bit float not supported at this time.", L"Error", MB_OK);
#else
						std::cout << "64 bit float not supported at this time.\n";
#endif
						return FALSE;
					default:
#ifdef _WINDOWS
						MessageBox(nullptr, L"Unknown float size.", L"Error", MB_OK);
#else
						std::cout << "Unknown float size.\n";
#endif
						return FALSE;
					}
				}
			}
			else {
#ifdef _WINDOWS
				MessageBox(nullptr, L"Unable to read X file header.", L"Error", MB_OK);
#else
				std::cout << "Unable to read X file header.\n";
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
				return text.ParseFile(hfile, btrack);
			}
		}
		return FALSE;
	}
}
