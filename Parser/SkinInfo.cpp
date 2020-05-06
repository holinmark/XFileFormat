#include "stdafx.h"

namespace ns_HoLin
{
	sSkinHeader::sSkinHeader()
	{
		nMaxSkinWeightsPerVertex = 0;
		nMaxSkinWeightsPerFace = 0;
		nBones = 0;
	}

	sSkinHeader::~sSkinHeader()
	{
		nMaxSkinWeightsPerVertex = 0;
		nMaxSkinWeightsPerFace = 0;
		nBones = 0;
	}

	sSkinWeights::sSkinWeights()
	{
		nWeights = 0;
		DirectX::XMStoreFloat4x4(&matrixOffset, DirectX::XMMatrixIdentity());
	}

	sSkinWeights::sSkinWeights(sSkinWeights &&other)
	{
		transformNodeName = other.transformNodeName;
		other.transformNodeName.clear();
		nWeights = other.nWeights;
		other.nWeights = 0;
		p_vertexIndices = std::move(other.p_vertexIndices);
		p_weights = std::move(other.p_weights);
		matrixOffset = other.matrixOffset;
		DirectX::XMStoreFloat4x4(&other.matrixOffset, DirectX::XMMatrixIdentity());
	}

	sSkinWeights::~sSkinWeights()
	{
		transformNodeName.clear();
		nWeights = 0;
		p_vertexIndices.clear();
		p_weights.clear();
		DirectX::XMStoreFloat4x4(&matrixOffset, DirectX::XMMatrixIdentity());
	}

	sSkinWeights& sSkinWeights::operator=(sSkinWeights &&other)
	{
		transformNodeName = other.transformNodeName;
		other.transformNodeName.clear();
		nWeights = other.nWeights;
		other.nWeights = 0;
		p_vertexIndices.clear();
		p_vertexIndices = std::move(other.p_vertexIndices);
		other.p_vertexIndices.clear();
		p_weights.clear();
		p_weights = std::move(other.p_weights);
		other.p_weights.clear();
		matrixOffset = other.matrixOffset;
		DirectX::XMStoreFloat4x4(&other.matrixOffset, DirectX::XMMatrixIdentity());
		return *this;
	}

	sSkinInfo::sSkinInfo()
	{
	}

	sSkinInfo::~sSkinInfo()
	{
		p_skin_weights.clear();
	}

	void sSkinInfo::CreateWeights(DWORD number_of_bones, DWORD max_transform_per_vertex, DWORD max_transform_per_face)
	{
		skin_header.nBones = number_of_bones;
		p_skin_weights.reserve(number_of_bones);
	}
}
