#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <fstream>
#include <cwchar>
#include "sMaterialList.h"
#include "sFrameList.h"
#include "sMeshList.h"
#include "cTextXFileParser.h"

void PrintMatrix(DirectX::XMFLOAT4X4);
void PrintMaterials(std::vector<ns_HoLin::sMaterial>&, ns_HoLin::sMesh*, const char*);
void PrintMesh(ns_HoLin::sMeshList*);
void PrintFrames(ns_HoLin::sSequenceOfFrames*);
void PrintAnimationSet(ns_HoLin::sAnimationSetList*);
void PrintDuplicates(ns_HoLin::sMesh*);
void PrintData(ns_HoLin::cTextXFileParser*);
ns_HoLin::cTextXFileParser* OpenFileWithMeshFileName();
BOOL ReadMeshFile(ns_HoLin::cTextXFileParser*);
BOOL GetXFileHeader(ns_HoLin::cTextXFileParser*, BOOL&, BOOL&);
int wmain(DWORD, const wchar_t**);
