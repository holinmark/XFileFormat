#pragma once

//#define FUNCTIONCALLSTACK

#include "targetver.h"

#include <Windows.h>
#include <DirectXMath.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#ifdef _CONSOLE
#include <process.h>
#endif

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
#include <tuple>

#include "cBinaryXFileParser.h"
#include "cTextXFileParser.h"
#include "SkinInfo.h"
#include "sMeshList.h"
#include "sMaterialList.h"
#include "sFrameList.h"
#include "sAnimationSet.h"
#include "sFunctionCallHistory.h"
#include "sErrorMessageException.h"
#include "sFileIO.h"
#include "cXFile.h"
