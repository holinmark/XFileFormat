#pragma once

#ifndef SANIMATIONSET_H
#define SANIMATIONSET_H

#include <DirectXMath.h>
#include <Windows.h>

#include <string>
#include <utility>
#include <vector>

namespace ns_HoLin
{
	struct sTimedFloatKeys
	{
		DWORD time;
		std::vector<float> tfkeys;

		sTimedFloatKeys();
		sTimedFloatKeys(sTimedFloatKeys&&) noexcept;
		sTimedFloatKeys(const sTimedFloatKeys&);
		~sTimedFloatKeys();
		void Cleanup(HANDLE = NULL);
		sTimedFloatKeys& operator=(sTimedFloatKeys&&) noexcept;
		sTimedFloatKeys& operator=(const sTimedFloatKeys&);
	};

	struct sAnimation_Data
	{
		// 0 for rotation DirectX::XMFLOAT3
		// 1 for scale DirectX::XMFLOAT3
		// 2 for translate DirectX::XMFLOAT3
		DWORD type_of_transform;
		std::vector<ns_HoLin::sTimedFloatKeys> transformation_data;
		sAnimation_Data *pnextanimation_data;

		sAnimation_Data();
		~sAnimation_Data();
		void Cleanup(HANDLE = NULL);
	};

	struct sAnimation
	{
		std::string name;
		std::vector<std::string> list_of_objects;
		sAnimation_Data *pfirst_data, *plast_data;
		sAnimation *pnextanimation;

		sAnimation();
		~sAnimation();
		void Cleanup(HANDLE = NULL);
		sAnimation_Data* CreateAnimation_Data();
		void InsertAnimation_Data(sAnimation_Data*);
	};

	struct sAnimationSet
	{
		std::string name;
		sAnimation *pfirstanimation, *plastanimation;
		sAnimationSet *pnext_set;

		sAnimationSet();
		~sAnimationSet();
		void Cleanup(HANDLE = NULL);
		sAnimation* CreateAnimation(); // creates an animation object to be stored inside an AnimationSet
		void Insert(sAnimation*);
		sAnimation* SearchForAnimation(std::string_view);
	};

	struct sAnimationSetList
	{
		DWORD animtickspersecond;
		sAnimationSet *pfirst_set, *plast_set;
		sAnimation *pfirst_animation, *plast_animation; // Any declared animation outside of AnimationSet is stored

		sAnimationSetList();
		~sAnimationSetList();
		void Cleanup(HANDLE = NULL);
		sAnimationSet* CreateAnimationSet();
		sAnimationSet* GetLastAnimationSet() { return plast_set; } // check if returns nullptr
		sAnimationSet* SearchForAnimationSet(std::string_view);
		sAnimation* SearchForAnimation(std::string_view);
		sAnimation* CreateAnimation(); // creates an animation object outside of an AnimationSet
		void InsertAnimation(sAnimation*);
	};
}

#endif
