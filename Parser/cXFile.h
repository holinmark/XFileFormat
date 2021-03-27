#pragma once

#ifndef CXFILE_H
#define CXFILE_H

#define XOFFILE_FORMAT_MAGIC			((long)'x' + ((long)'o' << 8) + ((long)'f' << 16) + ((long)' ' << 24))
#define XOFFILE_FORMAT_VERSION			((long)'0' + ((long)'3' << 8) + ((long)'0' << 16) + ((long)'3' << 24))
#define XOFFILE_FORMAT_BINARY			((long)'b' + ((long)'i' << 8) + ((long)'n' << 16) + ((long)' ' << 24))
#define XOFFILE_FORMAT_TEXT				((long)'t' + ((long)'x' << 8) + ((long)'t' << 16) + ((long)' ' << 24))
#define XOFFILE_FORMAT_COMPRESSED		((long)'c' + ((long)'m' << 8) + ((long)'p' << 16) + ((long)' ' << 24))
#define XOFFILE_FORMAT_FLOAT_BITS_32	((long)'0' + ((long)'0' << 8) + ((long)'3' << 16) + ((long)'2' << 24))
#define XOFFILE_FORMAT_FLOAT_BITS_64 	((long)'0' + ((long)'0' << 8) + ((long)'6' << 16) + ((long)'4' << 24))

#define TEXT_FILE		10000
#define BINARY_FILE		10001
#define ZIP_FILE		10002

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <array>
#include <unordered_map>
#include <string>
#include <utility>
#include <any>
#include "cTextXFileParser.h"

namespace ns_HoLin
{
	class cXFile
	{
		DWORD file_type;
		DWORD floatsize;
		HANDLE hfile;
		ns_HoLin::cTextXFileParser text;
	public:
		cXFile();
		~cXFile();
		void Cleanup();
		operator bool() {
			if (hfile)
				return true;
			return false;
		}
		BOOL ReadCommandLineArgumentsThenParse(DWORD, const wchar_t**);
		BOOL ReadXFile(const wchar_t*);
		DWORD GetXFileType() {
			return file_type;
		}
		ns_HoLin::cTextXFileParser* GetTextData() {
			return (ns_HoLin::cTextXFileParser*)&this->text;
		}
	protected:
		void OpenFileWithMeshFileName();
		bool openfile(const wchar_t*);
		BOOL ReadHeader();
		BOOL ParseFile(BOOL);
	};
}

#endif
