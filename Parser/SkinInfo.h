#pragma once

#ifndef SKININFO_H
#define SKININFO_H

namespace ns_HoLin
{
	struct sSkinHeader
	{
		DWORD nMaxSkinWeightsPerVertex;
		DWORD nMaxSkinWeightsPerFace;
		DWORD nBones;

		sSkinHeader();
		~sSkinHeader();
	};

	struct sSkinWeights
	{
		std::string transformNodeName;
		DWORD nWeights;
		std::vector<DWORD> p_vertexIndices;
		std::vector<float> p_weights;
		DirectX::XMFLOAT4X4 matrixOffset;

		sSkinWeights();
		sSkinWeights(sSkinWeights&&);
		~sSkinWeights();
		sSkinWeights& operator=(sSkinWeights&&);
	};

	struct sSkinInfo
	{
		sSkinHeader skin_header;
		std::vector<sSkinWeights> p_skin_weights;

		sSkinInfo();
		~sSkinInfo();
		void CreateWeights(DWORD, DWORD, DWORD);
	};
}

#endif
