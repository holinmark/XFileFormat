#pragma once

#define FUNCTIONCALLSTACK

#define XOFFILE_FORMAT_MAGIC				((long)'x' + ((long)'o' << 8) + ((long)'f' << 16) + ((long)' ' << 24))
#define XOFFILE_FORMAT_VERSION			((long)'0' + ((long)'3' << 8) + ((long)'0' << 16) + ((long)'3' << 24))
#define XOFFILE_FORMAT_BINARY				((long)'b' + ((long)'i' << 8) + ((long)'n' << 16) + ((long)' ' << 24))
#define XOFFILE_FORMAT_TEXT					((long)'t' + ((long)'x' << 8) + ((long)'t' << 16) + ((long)' ' << 24))
#define XOFFILE_FORMAT_COMPRESSED	((long)'c' + ((long)'m' << 8) + ((long)'p' << 16) + ((long)' ' << 24))
#define XOFFILE_FORMAT_FLOAT_BITS_32	((long)'0' + ((long)'0' << 8) + ((long)'3' << 16) + ((long)'2' << 24))
#define XOFFILE_FORMAT_FLOAT_BITS_64 ((long)'0' + ((long)'0' << 8) + ((long)'6' << 16) + ((long)'4' << 24))

#include "targetver.h"

#include <Windows.h>
#include <DirectXMath.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include <cctype>
#include <cwchar>
#include <iostream>
#include <fstream>
#include <string>
#include <array>
#include <any>
#include <vector>
#include <utility>
#include <limits>
#include <functional>
#include <new>
#include <cstdarg>
#include <unordered_map>

#include "sXFile.h"
#include "cTextXFileParser.h"
#include "SkinInfo.h"
#include "sMeshList.h"
#include "sMaterialList.h"
#include "sFrameList.h"
#include "sAnimationSet.h"
#include "sFunctionCallHistory.h"
#include "sErrorMessageException.h"
#include "sFileIO.h"
