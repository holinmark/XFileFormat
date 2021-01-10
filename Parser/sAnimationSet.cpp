#include "stdafx.h"

namespace ns_HoLin
{
	sTimedFloatKeys::sTimedFloatKeys()
	{
		time = 0;
	}

	sTimedFloatKeys::sTimedFloatKeys(sTimedFloatKeys &&other) noexcept
	{
		this->time = other.time;
		this->tfkeys = std::move(other.tfkeys);
	}
	
	sTimedFloatKeys::sTimedFloatKeys(const sTimedFloatKeys &other)
	{
		this->time = other.time;
		this->tfkeys.clear();
		std::copy(other.tfkeys.begin(), other.tfkeys.end(), this->tfkeys.begin());
	}

	sTimedFloatKeys::~sTimedFloatKeys()
	{
		Cleanup();
	}

	void sTimedFloatKeys::Cleanup(HANDLE hfile)
	{
		time = 0;
		tfkeys.clear();
	}

	sTimedFloatKeys& sTimedFloatKeys::operator=(sTimedFloatKeys &&other) noexcept
	{
		this->time = other.time;
		this->tfkeys = std::move(other.tfkeys);
		return *this;
	}
	
	sTimedFloatKeys& sTimedFloatKeys::operator=(const sTimedFloatKeys &other)
	{
		this->time = other.time;
		this->tfkeys.clear();
		std::copy(other.tfkeys.begin(), other.tfkeys.end(), this->tfkeys.begin());
		return *this;
	}
	
	sAnimation_Data::sAnimation_Data()
	{
		pnextanimation_data = nullptr;
	}

	sAnimation_Data::~sAnimation_Data()
	{
		sAnimation_Data::Cleanup();
	}

	void sAnimation_Data::Cleanup(HANDLE hfile)
	{
		if (pnextanimation_data) {
			delete pnextanimation_data;
			pnextanimation_data = nullptr;
		}
	}
	
	sAnimation::sAnimation()
	{
		pfirst_data = nullptr;
		plast_data = nullptr;
		pnextanimation = nullptr;
	}

	sAnimation::~sAnimation()
	{
		Cleanup();
	}
	
	void sAnimation::Cleanup(HANDLE hfile)
	{
		name.clear();
		list_of_objects.clear();
		if (pfirst_data) {
			delete pfirst_data;
			pfirst_data = nullptr;
			plast_data = nullptr;
		}
		if (pnextanimation) {
			delete pnextanimation;
			pnextanimation = nullptr;
		}
	}

	sAnimation_Data* sAnimation::CreateAnimation_Data()
	{
		sAnimation_Data *p = new sAnimation_Data;

		if (p) {
			if (plast_data) {
				plast_data->pnextanimation_data = p;
				plast_data = p;
			}
			else {
				pfirst_data = p;
				plast_data = p;
			}
		}
		else
			p = nullptr;
		return (sAnimation_Data*)p;
	}

	void sAnimation::InsertAnimation_Data(sAnimation_Data *p)
	{
		if (plast_data) {
			plast_data->pnextanimation_data = p;
			plast_data = p;
		}
		else {
			pfirst_data = p;
			plast_data = p;
		}
	}

	sAnimationSet::sAnimationSet()
	{
		pfirstanimation = nullptr;
		plastanimation = nullptr;
		pnext_set = nullptr;
	}

	sAnimationSet::~sAnimationSet()
	{
		Cleanup();
	}

	void sAnimationSet::Cleanup(HANDLE hfile)
	{
		name.clear();
		if (pfirstanimation) {
			delete pfirstanimation;
		}
		if (pnext_set) {
			delete pnext_set;
		}
		pfirstanimation = NULL;
		plastanimation = NULL;
		pnext_set = NULL;
	}

	sAnimation* sAnimationSet::CreateAnimation()
	{
		ns_HoLin::sAnimation *p_animation = (ns_HoLin::sAnimation*)new ns_HoLin::sAnimation;

		if (p_animation) {
			if (plastanimation) {
				plastanimation->pnextanimation = p_animation;
				plastanimation = p_animation;
			}
			else {
				pfirstanimation = p_animation;
				plastanimation = p_animation;
			}
		}
		else
			p_animation = nullptr;
		return (ns_HoLin::sAnimation*)p_animation;
	}

	void sAnimationSet::Insert(sAnimation *p)
	{
		if (plastanimation) {
			plastanimation->pnextanimation = p;
			plastanimation = p;
		}
		else {
			pfirstanimation = p;
			plastanimation = p;
		}
	}

	sAnimation* sAnimationSet::SearchForAnimation(std::string_view name)
	{
		if (pfirstanimation) {
			sAnimation *p = pfirstanimation;
			while (p) {
				if (p->name == name)
					return p;
				p = p->pnextanimation;
			}
		}
		return (sAnimation*)nullptr;
	}

	sAnimationSetList::sAnimationSetList()
	{
		animtickspersecond = 0;
		pfirst_set = nullptr;
		plast_set = nullptr;
		pfirst_animation = nullptr;
		plast_animation = nullptr;
	}

	sAnimationSetList::~sAnimationSetList()
	{
		Cleanup();
	}

	void sAnimationSetList::Cleanup(HANDLE hfile)
	{
		animtickspersecond = 0;
		if (pfirst_set) {
			delete pfirst_set;
			pfirst_set = nullptr;
			plast_set = nullptr;
		}
		if (pfirst_animation) {
			delete pfirst_animation;
			pfirst_animation = nullptr;
			plast_animation = nullptr;
		}
	}

	sAnimationSet* sAnimationSetList::CreateAnimationSet()
	{
		sAnimationSet *pset = (sAnimationSet*)new sAnimationSet;

		if (pset) {
			if (plast_set) {
				plast_set->pnext_set = pset;
				plast_set = pset;
			}
			else {
				pfirst_set = pset;
				plast_set = pset;
			}
		}
		else
			pset = nullptr;
		return (sAnimationSet*)pset;
	}

	sAnimationSet* sAnimationSetList::SearchForAnimationSet(std::string_view name)
	{
		if (pfirst_set) {
			sAnimationSet *p = pfirst_set;
			while (p) {
				if (p->name == name)
					return p;
				p = p->pnext_set;
			}
		}
		return (sAnimationSet*)nullptr;
	}

	sAnimation* sAnimationSetList::SearchForAnimation(std::string_view name)
	{
		if (pfirst_set) {
			sAnimationSet *pset = pfirst_set;
			sAnimation *panimation = pset->pfirstanimation;

			while (pset) {
				while (panimation) {
					if (panimation->name == name)
						return panimation;
					panimation = panimation->pnextanimation;
				}
				pset = pset->pnext_set;
			}
		}
		return (sAnimation*)nullptr;
	}

	sAnimation* sAnimationSetList::CreateAnimation()
	{
		sAnimation *p_animation = new sAnimation;
		
		if (p_animation) {
			if (plast_animation) {
				plast_animation->pnextanimation = p_animation;
				plast_animation = p_animation;
			}
			else {
				pfirst_animation = p_animation;
				plast_animation = p_animation;
			}
		}
		else {
			p_animation = nullptr;
		}
		return (sAnimation*)p_animation;
	}
	
	void sAnimationSetList::InsertAnimation(ns_HoLin::sAnimation *p)
	{
		if (plast_animation) {
			plast_animation->pnextanimation = p;
			plast_animation = p;
		}
		else {
			pfirst_animation = p;
			plast_animation = p;
		}
	}
}
