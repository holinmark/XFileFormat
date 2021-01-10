#pragma once

#include <DirectXMath.h>
#include <windows.h>
#include <vector>
#include <string>
#include <utility>

#include "SkinInfo.h"
#include "sAnimationSet.h"
#include "sMaterialList.h"

namespace ns_HoLin
{
	struct sMeshFaces
	{
		DWORD numofindices;
		std::vector<std::vector<DWORD>> facevertexindices;

		sMeshFaces();
		sMeshFaces(sMeshFaces&&);
		~sMeshFaces();
		sMeshFaces& operator=(sMeshFaces&&);
		void Cleanup(HANDLE = NULL);
	};

	struct sMeshNormals
	{
		std::vector<DirectX::XMFLOAT3> normals;
		std::vector<std::vector<DWORD>> facenormals;

		sMeshNormals();
		~sMeshNormals();
		void Cleanup(HANDLE = NULL);
	};

	struct sIndexedColor
	{
		DWORD index;
		DirectX::XMFLOAT4 indexColor; // 0 = r, 1 = g, 2 = b, 3 = a
	};

	struct sMeshVertexColors
	{
		std::vector<sIndexedColor> vertexColors;
		
		~sMeshVertexColors();
		void Cleanup(HANDLE = NULL);
	};

	struct sVertexDuplicationIndices
	{
		std::size_t nIndices; // Number of vertex indices. This is the number of vertices in the mesh.
		std::size_t nOriginalVertices; // Number of vertices in the mesh before any duplication occurs.
		std::vector<DWORD> Indices; /*The value indices[n] holds the vertex index that vertex[n]
										  in the vertex array for the mesh would have had if no duplication had occurred.
										  Indices in this array that are the same, therefore, indicate duplicate vertices.*/
		sVertexDuplicationIndices();
		~sVertexDuplicationIndices();
		void Cleanup(HANDLE = NULL);
	};

	// This data structure hasn't been incorporated into the program but will in the future.
	struct sMeshExtraAttributes
	{
		sMeshNormals meshnormals;
		std::vector<DirectX::XMFLOAT2> texturecoord;
		std::vector<DWORD> material_indices;
		std::vector<sMaterial> smeshmateriallist;
		sMeshVertexColors scolors;
		ns_HoLin::sVertexDuplicationIndices sduplicates;
		ns_HoLin::sAnimation_Data sanimation;
		ns_HoLin::sSkinInfo *p_skininfo;
		// if not empty, the first matrix applies to the mesh object
		// other matrices that follow are instances, if any, of that mesh object
		std::vector<DirectX::XMFLOAT4X4> list_of_matrices;

		sMeshExtraAttributes();
		~sMeshExtraAttributes();
		void Cleanup(HANDLE = NULL);
	};

	struct sMesh
	{
		std::string name;
		std::vector<DirectX::XMFLOAT3> vertices;
		sMeshFaces meshfaces;

		// sMeshExtraAttributes structure goes here
		ns_HoLin::sMeshExtraAttributes *p_extra;
		// pointer to the next mesh
		ns_HoLin::sMesh *pnextmesh;
		//static std::size_t number_of_mesh_created;

		sMesh();
		sMesh(sMesh&&);
		~sMesh();
		sMesh& operator=(sMesh&&);
		void Cleanup();
	};

	struct sMeshList
	{
		std::size_t number_of_meshes;
		sMesh *pfirstmesh, *plastmesh;

		sMeshList();
		~sMeshList();
		void Cleanup();
		sMesh* CreateNewMesh();
		void InsertMesh(sMesh* = nullptr);
		BOOL Find(std::string_view);
		sMesh* Search(std::string_view);
		sMesh* GetLastMesh() { return plastmesh; }
	};
}
