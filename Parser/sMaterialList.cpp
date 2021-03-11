#include "stdafx.h"

namespace ns_HoLin
{
	sMaterial::sMaterial()
	{
		facecolor = DirectX::XMFLOAT4(1, 1, 1, 1);
		power = 1.0f;
		specularcolor = emissivecolor = DirectX::XMFLOAT3(0.1f, 0.1f, 0.1f);
	}

	sMaterial::~sMaterial()
	{
		name.clear();
		filename.clear();
	}

	sMaterial::sMaterial(const sMaterial &other)
	{
		name = other.name;
		facecolor = other.facecolor;
		power = other.power;
		specularcolor = other.specularcolor;
		emissivecolor = other.emissivecolor;
		filename = other.filename;
	}

	sMaterial::sMaterial(sMaterial &&other)
	{
		name = other.name;
		other.name.clear();
		facecolor = other.facecolor;
		power = other.power;
		specularcolor = other.specularcolor;
		emissivecolor = other.emissivecolor;
		filename = other.filename;
		other.filename.clear();
	}

	sMaterial& sMaterial::operator=(sMaterial &&other)
	{
		name = other.name;
		other.name.clear();
		facecolor = other.facecolor;
		power = other.power;
		specularcolor = other.specularcolor;
		emissivecolor = other.emissivecolor;
		filename = other.filename;
		other.filename.clear();
		return *this;
	}
	/*
	sMaterialList::sMaterialList()
	{
		pfirstmaterial = nullptr;
		plastmaterial = nullptr;
		number_of_materials = 0;
	}

	sMaterialList::~sMaterialList()
	{
		if (pfirstmaterial) {
			delete pfirstmaterial;
			pfirstmaterial = nullptr;
			plastmaterial = nullptr;
		}
		number_of_materials = 0;
	}

	sMaterial* sMaterialList::CreateNewMaterial()
	{
		sMaterial *p = new sMaterial;

		if (p) {
			if (plastmaterial) {
				plastmaterial->pnextmaterial = p;
				plastmaterial = p;
			}
			else {
				pfirstmaterial = p;
				plastmaterial = p;
			}
			number_of_materials++;
		}
		return (sMaterial*)p;
	}

	void sMaterialList::InsertMaterial(sMaterial *pmaterial)
	{
		if (plastmaterial) {
			plastmaterial->pnextmaterial = pmaterial;
			plastmaterial = pmaterial;
		}
		else {
			pfirstmaterial = pmaterial;
			plastmaterial = pmaterial;
		}
		number_of_materials++;
	}

	sMaterial* sMaterialList::Search(const char *name)
	{
		if (pfirstmaterial) {
			sMaterial *ptmp = pfirstmaterial;
			while (ptmp) {
				if (ptmp->name.size() > 0) {
					if (strcmp(ptmp->name.c_str(), name) == 0)
						return ptmp;
				}
				ptmp = ptmp->pnextmaterial;
			}
		}
		return (sMaterial*)nullptr;
	}

	BOOL sMaterialList::Find(std::string name)
	{
		if (pfirstmaterial) {
			sMaterial *p = pfirstmaterial;

			while (p) {
				if (p->name == name)
					return TRUE;
				p = p->pnextmaterial;
			}
		}
		return FALSE;
	}
	*/
}
