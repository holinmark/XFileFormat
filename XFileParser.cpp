// XFileParser.cpp : Defines the entry point for the console application.
//

#include "XFileParser.h"
#include "stdafx.h"

void PrintMatrix(DirectX::XMFLOAT4X4 matrix)
{
	for (std::size_t row = 0; row < 4; ++row) {
		for (std::size_t col = 0; col < 4; ++col) {
			std::clog << matrix.m[row][col] << ", ";
		}
		std::clog << '\n';
	}
	std::clog << '\n\n';
}

void PrintMaterials(std::vector<ns_HoLin::sMaterial> &materials, ns_HoLin::sMesh *p_mesh, const char *pstr)
{
	if (materials.size() > 0) {
		std::clog << pstr << '\n';
		for (std::size_t i = 0; i < materials.size(); ++i) {
			std::clog << "\tMaterial name: " << materials[i].name.c_str() << '\n';
			std::clog << "\tFace color " << materials[i].facecolor.x << ", ";
			std::clog << materials[i].facecolor.y << ", " << materials[i].facecolor.z << ", " << materials[i].facecolor.w << '\n';
			std::clog << "\tPower" << materials[i].power << '\n';
			std::clog << "\tSpecular " << materials[i].specularcolor.x << ", ";
			std::clog << materials[i].specularcolor.y << ", " << materials[i].specularcolor.z << '\n';
			std::clog << "\tEmmisive " << materials[i].emissivecolor.x << ", " << materials[i].emissivecolor.y;
			std::clog << materials[i].emissivecolor.z << '\n';
			std::clog << "\tFile name: " << materials[i].filename.c_str() << "\n\n";
		}
	}
	std::clog << std::endl;
	while (p_mesh) {
		std::clog << "Mesh name: " << p_mesh->name.c_str() << '\n';
		if (p_mesh->p_extra) {
			for (std::size_t i = 0; i < p_mesh->p_extra->smeshmateriallist.size(); ++i) {
				std::clog << "\tMaterial name: " << p_mesh->p_extra->smeshmateriallist[i].name.c_str() << '\n';
				std::clog << "\tFace color " << p_mesh->p_extra->smeshmateriallist[i].facecolor.x << ", ";
				std::clog << p_mesh->p_extra->smeshmateriallist[i].facecolor.y << ", " << p_mesh->p_extra->smeshmateriallist[i].facecolor.z << ", ";
				std::clog << p_mesh->p_extra->smeshmateriallist[i].facecolor.w << '\n';
				std::clog << "\tPower " << p_mesh->p_extra->smeshmateriallist[i].power << '\n';
				std::clog << "\tSpecular " << p_mesh->p_extra->smeshmateriallist[i].specularcolor.x << ", ";
				std::clog << p_mesh->p_extra->smeshmateriallist[i].specularcolor.y << ", " << p_mesh->p_extra->smeshmateriallist[i].specularcolor.z << '\n';
				std::clog << "\tEmmisive " << p_mesh->p_extra->smeshmateriallist[i].emissivecolor.x << ", ";
				std::clog << p_mesh->p_extra->smeshmateriallist[i].emissivecolor.y << ", " << p_mesh->p_extra->smeshmateriallist[i].emissivecolor.z << '\n';
				std::clog << "\tFile name: " << p_mesh->p_extra->smeshmateriallist[i].filename.c_str() << '\n';
			}
		}
		p_mesh = p_mesh->pnextmesh;
	}
	std::puts("");
}

void PrintMesh(ns_HoLin::sMeshList *p_list_of_meshes)
{
	ns_HoLin::sMesh *pmesh = p_list_of_meshes->pfirstmesh;
	
	if (pmesh) {
		std::clog << "Number of meshes " << p_list_of_meshes->number_of_meshes << '\n';
		while (pmesh) {
			std::clog << "\tMesh " << pmesh->name.c_str() << '\n';
			if (pmesh->p_extra) {
				if (pmesh->p_extra->p_skininfo) {
					for (std::size_t i=0; i<pmesh->p_extra->p_skininfo->p_skin_weights.size(); ++i) {
						if (pmesh->p_extra->p_skininfo->p_skin_weights[i].p_weights.size() == pmesh->p_extra->p_skininfo->p_skin_weights[i].p_vertexIndices.size()) {
							for (std::size_t j=0; j<pmesh->p_extra->p_skininfo->p_skin_weights[i].p_weights.size(); ++j) {
								//std::clog << pmesh->p_extra->p_skininfo->p_skin_weights[i].p_vertexIndices[j] << " : " << pmesh->p_extra->p_skininfo->p_skin_weights[i].p_weights[j] << '\n';
							}
						}
					}
				}
			}
			pmesh = pmesh->pnextmesh;
		}
	}
	else {
		std::clog << "\nNo mesh\n\n";
	}
}

void PrintFrames(ns_HoLin::sSequenceOfFrames *pframeseq)
{
	if (pframeseq != nullptr) {
		std::size_t index = 1;
		ns_HoLin::sFrame *pframe;
		
		std::clog << "\n\nFrames :\n";
		while (pframeseq) {
			std::clog << "Keys in frame sequences.\n";
			for (auto n : pframeseq->framenames) {
				std::clog << n.c_str() << ", ";
			}
			std::clog << std::endl;
			pframe = pframeseq->pfirstframe;
			while (pframe) {
				std::clog << "\tFrame : " << pframe->name.c_str() << '\n';
				if (pframe->parent_name.length() > 0)
					std::clog << "\t\tParent : " << pframe->parent_name.c_str() << '\n';
				else
					std::clog << "\t\tNo parent" << '\n';
				if (pframeseq->parent_children[pframe->name].empty() == false) {
					std::clog << "\t\tChildren " << pframeseq->parent_children[pframe->name].size() << "\n\t\t";
					for (auto c : pframeseq->parent_children[pframe->name]) {
						std::clog << c.c_str() << ", ";
					}
					std::clog << std::endl;
				}
				if (pframe->mesh.size() > 0) {
					std::clog << "Meshes\n";
					for (auto m : pframe->mesh) {
						std::clog << m.c_str() << ", ";
					}
std::clog << '\n';
				}
				pframe = pframe->pnextframe;
			}
			std::clog << '\n';
			pframeseq = pframeseq->pnextseq;
		}
	}
	else {
	std::clog << "\nNo frames\n\n";
	}
}

void PrintAnimationSet(ns_HoLin::sAnimationSetList *psetlist)
{
	std::clog << "\nAnimation data:\n";
	if (psetlist) {
		ns_HoLin::sAnimationSet *p_anim_set = psetlist->pfirst_set;
		ns_HoLin::sAnimation *p_anim = nullptr;
		ns_HoLin::sAnimation_Data *p = nullptr;
		std::size_t number_of_animations = 0;

		p_anim = psetlist->pfirst_animation;
		while (p_anim) {
			std::clog << "\tAnimation " << p_anim->name << '\n';
			p_anim = p_anim->pnextanimation;
		}
		p_anim_set = psetlist->pfirst_set;
		while (p_anim_set) {
			std::clog << "\tAnimation set " << p_anim_set->name << '\n';
			p_anim = p_anim_set->pfirstanimation;
			while (p_anim) {
				number_of_animations++;
				std::clog << "\t\tAnimation " << p_anim->name << '\n';
				p = p_anim->pfirst_data;
				while (p) {
					std::clog << "\t\t\tTransform type " << p->type_of_transform << '\n';
					for (DWORD i = 0; i < p->transformation_data.size(); ++i) {
						std::clog << p->transformation_data[i].time << " : ";
						for (DWORD j = 0; j < p->transformation_data[i].tfkeys.size(); ++j) {
							std::clog << p->transformation_data[i].tfkeys[j] << ", ";
						}
						std::clog << '\n';
					}
					std::clog << '\n';
					p = p->pnextanimation_data;
				}
				p_anim = p_anim->pnextanimation;
			}
			p_anim_set = p_anim_set->pnext_set;
		}
		std::clog << "\nNumber of animations " << number_of_animations << '\n';
	}
}

void PrintDuplicates(ns_HoLin::sMesh *p_mesh)
{
	while (p_mesh) {
		std::cout << p_mesh->name.c_str() << '\n';
		if (p_mesh->p_extra) {
			for (auto i : p_mesh->p_extra->sduplicates.Indices) {
				std::cout << i << ", ";
			}
			std::cout << '\n' << p_mesh->p_extra->sduplicates.nIndices << " " << p_mesh->p_extra->sduplicates.nOriginalVertices << "\n\n";
		}
		p_mesh = p_mesh->pnextmesh;
	}
}

void PrintData(ns_HoLin::cTextXFileParser *p_xfile)
{
	ns_HoLin::sAnimationSetList *pset = &p_xfile->xfiledata.sanimationsetlist;

	PrintMesh(&p_xfile->xfiledata.smeshlist);
	PrintFrames(p_xfile->xfiledata.sframeslist.pfirstseq);
	PrintMaterials(p_xfile->xfiledata.smateriallist, p_xfile->xfiledata.smeshlist.pfirstmesh, "Global materials");
	//PrintAnimationSet(&p_xfile->xfiledata.sanimationsetlist);
	//PrintDuplicates(p_xfile->xfiledata.smeshlist.pfirstmesh);
}

int wmain(DWORD argv, const wchar_t **argc)
{
	ns_HoLin::cXFile xfile;

#if defined(_WINDOWS)
	// Win32 application
	if (xfile.ReadXFile(L"mesh//Five_Wheeler_mesh.txt")) {
		if (xfile.GetXFileType() == TEXT_FILE) {
			ns_HoLin::cTextXFileParser *p = xfile.GetTextData();
	
			if (p) {
				PrintData(p);
			}
		}
	}
	MessageBox(nullptr, L"Terminating program.", L"End", MB_OK);
#else
	// Console application
	if (xfile.ReadCommandLineArgumentsThenParse(argv, argc)) {
		std::wcout << L"Success.\n";
		if (xfile.GetXFileType() == TEXT_FILE) {
			ns_HoLin::cTextXFileParser *p = xfile.GetTextData();
			if (p) {
				PrintData(p);
			}
			else {
				std::wcout << L"Error pointer.\n";
			}
		}
	}
	std::wcout << L"Closing program.\n";
#endif
	return 1;
}
