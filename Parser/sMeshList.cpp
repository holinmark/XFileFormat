#include "stdafx.h"

namespace ns_HoLin
{
	sMeshFaces::sMeshFaces()
	{
		numofindices = 0;
	}

	sMeshFaces::~sMeshFaces()
	{
		Cleanup();
	}

	void sMeshFaces::Cleanup(HANDLE hfile)
	{
		numofindices = 0;
		facevertexindices.clear();
	}

	sMeshNormals::sMeshNormals()
	{
	}

	sMeshNormals::~sMeshNormals()
	{
		Cleanup();
	}

	void sMeshNormals::Cleanup(HANDLE hfile)
	{
		normals.clear();
		facenormals.clear();
	}
	
	sMeshVertexColors::~sMeshVertexColors()
	{
		Cleanup();
	}
	
	void sMeshVertexColors::Cleanup(HANDLE hfile)
	{
		vertexColors.clear();
	}
	
	sVertexDuplicationIndices::sVertexDuplicationIndices()
	{
		nIndices = 0;
		nOriginalVertices = 0;
	}
	
	sVertexDuplicationIndices::~sVertexDuplicationIndices()
	{
		Cleanup();
	}

	void sVertexDuplicationIndices::Cleanup(HANDLE hfile)
	{
		nIndices = 0;
		nOriginalVertices = 0;
		Indices.clear();
	}

	sMeshExtraAttributes::sMeshExtraAttributes()
	{
		p_skininfo = NULL;
	}

	sMeshExtraAttributes::~sMeshExtraAttributes()
	{
		Cleanup();
	}

	void sMeshExtraAttributes::Cleanup(HANDLE hfile)
	{
		meshnormals.Cleanup(hfile);
		texturecoord.clear();
		material_indices.clear();
		smeshmateriallist.clear();
		scolors.Cleanup();
		sanimation.Cleanup();
		if (p_skininfo) {
			delete p_skininfo;
			p_skininfo = NULL;
		}
		list_of_matrices.clear();
	}

	std::size_t sMesh::number_of_mesh_created(0);

	sMesh::sMesh()
	{
		number_of_mesh_created++;
		p_extra = nullptr;
		//p_skininfo = nullptr;
		pnextmesh = nullptr;
	}

	sMesh::~sMesh()
	{
		Cleanup();
	}

	void sMesh::Cleanup()
	{
		name.clear();
		vertices.clear();
		meshfaces.Cleanup();
		if (p_extra)
			delete p_extra;
		p_extra = nullptr;
		number_of_mesh_created--;
		if (pnextmesh) {
			delete pnextmesh;
			pnextmesh = nullptr;
		}
	}

	sMeshList::sMeshList()
	{
		number_of_meshes = 0;
		pfirstmesh = nullptr;
		plastmesh = nullptr;
	}

	sMeshList::~sMeshList()
	{
		Cleanup();
	}

	void sMeshList::Cleanup()
	{
		number_of_meshes = 0;
		if (pfirstmesh) {
			delete pfirstmesh;
			pfirstmesh = nullptr;
			plastmesh = nullptr;
		}
	}

	sMesh* sMeshList::CreateNewMesh()
	{
		sMesh *p_tmp = (sMesh*)new sMesh;

		if (p_tmp) {
			if (plastmesh) {
				plastmesh->pnextmesh = p_tmp;
				plastmesh = p_tmp;
			}
			else {
				pfirstmesh = p_tmp;
				plastmesh = p_tmp;
			}
			number_of_meshes++;
		}
		return (sMesh*)p_tmp;
	}

	void sMeshList::InsertMesh(sMesh *pmesh)
	{
		if (pmesh) {
			if (pfirstmesh) {
				plastmesh->pnextmesh = pmesh;
				plastmesh = pmesh;
			}
			else {
				pfirstmesh = pmesh;
				plastmesh = pmesh;
			}
			number_of_meshes++;
		}
	}

	BOOL sMeshList::Find(std::string_view name)
	{
		if (pfirstmesh) {
			sMesh *ptmp = pfirstmesh;

			while (ptmp) {
				if (ptmp->name.size() > 0) {
					if (ptmp->name == name) {
						return TRUE;
					}
				}
				ptmp = ptmp->pnextmesh;
			}
		}
		return FALSE;
	}
	
	sMesh* sMeshList::Search(std::string_view name)
	{
		if (pfirstmesh) {
			sMesh *pmesh = pfirstmesh;
			while (pmesh) {
				if (pmesh->name == name)
					return pmesh;
				pmesh = pmesh->pnextmesh;
			}
		}
		return (sMesh*)nullptr;
	}
}
