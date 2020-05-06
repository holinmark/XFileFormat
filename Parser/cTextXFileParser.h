#pragma once

#ifndef CTEXTXFILEPARSER_H
#define CTEXTXFILEPARSER_H

#include <DirectXMath.h>
#include <Windows.h>
#include <cstdio>
#include <string>
#include <vector>
#include <tuple>
#include <limits>
#include <functional>
#include <new>
#include <cctype>
#include <cstdarg>
#include <fstream>

#include "sFileIO.h"
#include "sMeshList.h"
#include "sMaterialList.h"
#include "sFrameList.h"
#include "sAnimationSet.h"
#include "sFunctionCallHistory.h"
#include "sErrorMessageException.h"

namespace ns_HoLin
{
	struct sXFileData
	{
		std::vector<ns_HoLin::sMaterial> smateriallist;
		ns_HoLin::sMeshList smeshlist;
		ns_HoLin::sFrames sframeslist;
		ns_HoLin::sAnimationSetList sanimationsetlist;
	};
	
	class cTextXFileParser
	{
	public:
		ns_HoLin::sFileIO sfile;
		ns_HoLin::sXFileData xfiledata;
		ns_HoLin::sErrorMessageException serrormessages;
		BOOL trackoutput;
		BOOL endoffile;
		
		cTextXFileParser(const wchar_t*);
		~cTextXFileParser();
		BOOL ParseFile();
		explicit operator bool() const;
	private:
		std::size_t linenumber;
		ns_HoLin::sFunctionCallHistory functioncalls;
		
		BOOL GetChar();
		BOOL GetXFileHeader();
		BOOL GetTemplateName(char*, std::size_t);
		BOOL GetDigit(char*, std::size_t);
		BOOL GetSignedDigit(char*, std::size_t);
		BOOL GetMatrixBody(char*, std::size_t, void*, DWORD, DWORD);
		BOOL GetMatrix(char*, std::size_t, DirectX::XMFLOAT4X4&);
		BOOL GetFrameTransformMatrix(char*, std::size_t, DirectX::XMFLOAT4X4&);
		BOOL GetReservedWord(char*, std::size_t, char);
		BOOL GetName(char*, std::size_t, char);
		BOOL GetString(char*, std::size_t, char);
		BOOL GetColorRGBA(char*, std::size_t, DirectX::XMFLOAT4&);
		BOOL GetColorRGB(char*, std::size_t, DirectX::XMFLOAT3&);
		BOOL GetMaterialAttributes(char*, std::size_t, ns_HoLin::sMaterial*);
		BOOL GetMaterialBody(char*, std::size_t, ns_HoLin::sMaterial*);
		BOOL GetMeshMaterialListAttributes(char*, std::size_t, ns_HoLin::sMesh*);
		BOOL GetMeshMaterialListBody(char*, std::size_t, ns_HoLin::sMesh*);
		BOOL GetUnsignedInteger(char*, std::size_t, void*);
		BOOL GetMeshMaterialList(char*, std::size_t, ns_HoLin::sMesh*);
		BOOL GetMaterial(char*, std::size_t, ns_HoLin::sMaterial*);
		BOOL GetUnsignedInteger(char*, std::size_t, BOOL = FALSE);
		BOOL GetExponent(char*, std::size_t);
		BOOL GetFraction(char*, std::size_t);
		BOOL GetInteger(char*, std::size_t);
		BOOL GetFloat(char*, std::size_t, BOOL = FALSE);
		BOOL GetFloat(char*, std::size_t, void*);
		BOOL GetVector(char*, std::size_t, void*);
		BOOL GetPolygon(char*, std::size_t, void*);
		BOOL GetPolygons(char*, std::size_t, void*);
		BOOL GetFaces(char*, std::size_t, ns_HoLin::sMeshFaces&);
		BOOL GetArray(char*, std::size_t, void*, DWORD, BOOL(cTextXFileParser::*pCallBackFunction)(char*, std::size_t, void*), BOOL = TRUE);
		BOOL Get2DArray(char*, std::size_t, void*, DWORD, DWORD, BOOL(cTextXFileParser::*pCallBackFunction)(char*, std::size_t, void*, DWORD, DWORD));
		BOOL GetVertices(char*, std::size_t, std::vector<DirectX::XMFLOAT3>&, DWORD);
		BOOL GetNormalFaceIndices(char*, std::size_t, void*);
		BOOL GetMeshNormalsBody(char*, std::size_t, ns_HoLin::sMeshNormals*);
		BOOL GetMeshNormals(char*, std::size_t, ns_HoLin::sMeshNormals*);
		BOOL GetCoord2D(char*, std::size_t, void*);
		BOOL GetMeshTextureCoordBody(char*, std::size_t, std::vector<DirectX::XMFLOAT2>&);
		BOOL GetMeshTextureCoord(char*, std::size_t, std::vector<DirectX::XMFLOAT2>&);
		BOOL GetSkinWeightsBody(char*, std::size_t, ns_HoLin::sMesh*);
		BOOL GetSkinWeights(char*, std::size_t, ns_HoLin::sMesh*);
		BOOL GetXSkinMeshHeaderBody(char*, std::size_t, ns_HoLin::sMesh*);
		BOOL GetXSkinMeshHeader(char*, std::size_t, ns_HoLin::sMesh*);
		BOOL GetVertexDuplicationIndicesBody(char*, std::size_t, ns_HoLin::sMesh*);
		BOOL GetVertexDuplicationIndices(char*, std::size_t, ns_HoLin::sMesh*);
		BOOL GetIndexColor(char*, std::size_t, void*);
		BOOL GetMeshVertexColorsBody(char*, std::size_t, ns_HoLin::sMesh*);
		BOOL GetMeshVertexColors(char*, std::size_t, ns_HoLin::sMesh*);
		BOOL GetMeshAttributes(char*, std::size_t, ns_HoLin::sMesh*);
		BOOL GetMeshBody(char*, std::size_t, ns_HoLin::sMesh*);
		BOOL GetMesh(char*, std::size_t, ns_HoLin::sMesh*);
		BOOL GetSubFrame(char*, std::size_t, ns_HoLin::sFrame*);
		BOOL GetFrameAttributes(char*, std::size_t, ns_HoLin::sFrame*);
		BOOL GetFrameBody(char*, std::size_t, ns_HoLin::sFrame*);
		BOOL GetFrame(char*, std::size_t, ns_HoLin::sSequenceOfFrames*);
		BOOL GetFloatKeysBody(char*, std::size_t, void*);
		BOOL GetTimedFloatKeys(char*, std::size_t, void*);
		BOOL GetAnimationKeyBody(char*, std::size_t, ns_HoLin::sAnimation_Data*);
		BOOL GetAnimationKey(char*, std::size_t, ns_HoLin::sAnimation_Data*);
		BOOL GetAnimationOptionsBody(char*, std::size_t);
		BOOL GetAnimationOptions(char*, std::size_t);
		BOOL GetAnimationBody(char*, std::size_t, ns_HoLin::sAnimation*);
		BOOL GetAnimation(char*, std::size_t, ns_HoLin::sAnimationSet * = nullptr);
		BOOL GetAnimationSetBody(char*, std::size_t, ns_HoLin::sAnimationSet*);
		BOOL GetAnimationSet(char*, std::size_t);
		BOOL GetAnimTicksPerSecond(char*, std::size_t);
		BOOL ExtractTemplates(char*, std::size_t);
		BOOL Recovery(char*, std::size_t, char, char);
		BOOL VerifyToken(char);
		BOOL GetCarriageReturn();
		BOOL ReadCommentChar();
		BOOL GetComment();
		BOOL GetNextInput(std::function<BOOL(int)>);
		BOOL GetNextToken(const char);
		void GetOffendingLine(std::string&, std::string&);
		BOOL PrintOffendingLine(const char*, ...);
		
		friend BOOL IsWhiteSpace(ns_HoLin::cTextXFileParser*, int);
	};

	BOOL IsValidEntry(int);
	BOOL IsValidSeperator(int);
	BOOL IsValidCharOpening(int);
	BOOL IsValidCharClosing(int);
	BOOL CreateName(char*, std::size_t);
}

#endif
