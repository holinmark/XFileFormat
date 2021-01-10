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

	sSkinWeights::sSkinWeights(sSkinWeights &&other) noexcept
	{
		transformNodeName = std::move(other.transformNodeName);
		nWeights = other.nWeights;
		other.nWeights = 0;
		p_vertexIndices = std::move(other.p_vertexIndices);
		p_weights = std::move(other.p_weights);
		matrixOffset = other.matrixOffset;
		DirectX::XMStoreFloat4x4(&other.matrixOffset, DirectX::XMMatrixIdentity());
	}
	
	sSkinWeights::sSkinWeights(const sSkinWeights &other) noexcept
	{
		transformNodeName = other.transformNodeName;
		nWeights = other.nWeights;
		p_vertexIndices.clear();
		std::copy(other.p_vertexIndices.begin(), other.p_vertexIndices.end(), p_vertexIndices.begin());
		p_weights.clear();
		std::copy(other.p_weights.begin(), other.p_weights.end(), p_weights.begin());
		matrixOffset = other.matrixOffset;
	}
	
	sSkinWeights::~sSkinWeights()
	{
		transformNodeName.clear();
		nWeights = 0;
		p_vertexIndices.clear();
		p_weights.clear();
		DirectX::XMStoreFloat4x4(&matrixOffset, DirectX::XMMatrixIdentity());
	}

	sSkinWeights& sSkinWeights::operator=(sSkinWeights &&other) noexcept
	{
		this->transformNodeName = std::move(other.transformNodeName);
		this->nWeights = other.nWeights;
		other.nWeights = 0;
		this->p_vertexIndices.clear();
		this->p_vertexIndices = std::move(other.p_vertexIndices);
		this->p_weights.clear();
		this->p_weights = std::move(other.p_weights);
		this->matrixOffset = other.matrixOffset;
		DirectX::XMStoreFloat4x4(&other.matrixOffset, DirectX::XMMatrixIdentity());
		return *this;
	}

	sSkinWeights& sSkinWeights::operator=(const sSkinWeights &other)
	{
		this->transformNodeName = other.transformNodeName;
		this->nWeights = other.nWeights;
		this->p_vertexIndices.clear();
		std::copy(other.p_vertexIndices.begin(), other.p_vertexIndices.end(), this->p_vertexIndices.begin());
		this->p_weights.clear();
		std::copy(other.p_weights.begin(), other.p_weights.end(), this->p_weights.begin());
		this->matrixOffset = other.matrixOffset;
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
