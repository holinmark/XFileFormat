#pragma once

#include <string>
#include <DirectXMath.h>

namespace ns_HoLin
{
	struct sMaterial
	{
		std::string name;
		DirectX::XMFLOAT4 facecolor;
		float power;
		DirectX::XMFLOAT3 specularcolor, emissivecolor;
		std::string filename;

		sMaterial();
		~sMaterial();
		sMaterial(sMaterial&&);
		sMaterial& operator=(sMaterial&&);
	};
/*
	struct sMaterialList
	{
		sMaterial *pfirstmaterial, *plastmaterial;
		DWORD number_of_materials;

		sMaterialList();
		~sMaterialList();

		sMaterial* CreateNewMaterial();
		void InsertMaterial(sMaterial*);
		sMaterial* Search(const char*);
		BOOL Find(std::string);
	};*/
}
