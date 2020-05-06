#include "stdafx.h"

namespace ns_HoLin
{
	std::size_t sFrame::number_of_frames_created(0);

	sFrame::sFrame()
	{
		number_of_frames_created++;
		pnextframe = nullptr;
		DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixIdentity());
	}

	sFrame::~sFrame()
	{
		Cleanup();
	}

	void sFrame::Cleanup(HANDLE hfile)
	{
		name.clear();
		mesh.clear();
		vectors.clear();
		DirectX::XMStoreFloat4x4(&matrix, DirectX::XMMatrixIdentity());
		parent_name.clear();
		if (pnextframe) {
			delete pnextframe;
			pnextframe = nullptr;
		}
		number_of_frames_created--;
	}

	sSequenceOfFrames::sSequenceOfFrames()
	{
		pfirstframe = nullptr;
		plastframe = nullptr;
		pnextseq = nullptr;
		number_of_frames_in_sequence = 0;
	}

	sSequenceOfFrames::~sSequenceOfFrames()
	{
		Cleanup();
	}

	void sSequenceOfFrames::Cleanup(HANDLE hfile)
	{
		if (pfirstframe) {
			delete pfirstframe;
			pfirstframe = nullptr;
			plastframe = nullptr;
		}
		framenames.clear();
		parent_children.clear();
		number_of_frames_in_sequence = 0;
		if (pnextseq) {
			delete pnextseq;
			pnextseq = nullptr;
		}
	}

	void sSequenceOfFrames::InsertFrame(sFrame *pframe)
	{
		if (plastframe) {
			plastframe->pnextframe = pframe;
			plastframe = pframe;
		}
		else {
			pfirstframe = pframe;
			plastframe = pframe;
		}
		number_of_frames_in_sequence++;
	}

	BOOL sSequenceOfFrames::Find(std::string_view name)
	{
		ns_HoLin::sFrame *p_tmp = pfirstframe;

		while (p_tmp) {
			if (p_tmp->name == name)
				return TRUE;
			p_tmp = p_tmp->pnextframe;
		}
		return FALSE;
	}
	
	ns_HoLin::sFrame* sSequenceOfFrames::Search(std::string_view name)
	{
		ns_HoLin::sFrame *p_tmp = pfirstframe;

		while (p_tmp) {
			if (p_tmp->name == name)
				return p_tmp;
			p_tmp = p_tmp->pnextframe;
		}
		return (ns_HoLin::sFrame*)nullptr;
	}

	ns_HoLin::sFrame* sSequenceOfFrames::CreateNewFrame()
	{
		ns_HoLin::sFrame *p_tmp = new ns_HoLin::sFrame;

		if (p_tmp) {
			if (plastframe) {
				plastframe->pnextframe = p_tmp;
				plastframe = p_tmp;
			}
			else {
				pfirstframe = p_tmp;
				plastframe = p_tmp;
			}
			number_of_frames_in_sequence++;
		}
		return (ns_HoLin::sFrame*)p_tmp;
	}

	ns_HoLin::sFrame* sSequenceOfFrames::GetLastEntry()
	{
		if (plastframe)
			return plastframe;
		return (ns_HoLin::sFrame*)nullptr;
	}

	sFrames::sFrames()
	{
		number_of_sequences = 0;
		pfirstseq = nullptr;
		plastseq = nullptr;
	}

	sFrames::~sFrames()
	{
		Cleanup();
	}

	void	sFrames::Cleanup(HANDLE hfile)
	{
		if (pfirstseq) {
			delete pfirstseq;
			pfirstseq = nullptr;
			plastseq = nullptr;
		}
		number_of_sequences = 0;
	}

	ns_HoLin::sSequenceOfFrames *sFrames::CreateNewSequence()
	{
		ns_HoLin::sSequenceOfFrames *pseq = new ns_HoLin::sSequenceOfFrames;

		if (pseq) {
			InsertSequenceOfFrames(pseq);
			return pseq;
		}
		return (ns_HoLin::sSequenceOfFrames*)nullptr;
	}

	void sFrames::InsertSequenceOfFrames(ns_HoLin::sSequenceOfFrames *pseq)
	{
		if (plastseq) {
			plastseq->pnextseq = pseq;
			plastseq = pseq;
		}
		else {
			pfirstseq = plastseq = pseq;
		}
		number_of_sequences++;
	}

	void sFrames::InsertFrameIntoLastSequence(ns_HoLin::sFrame *pframe)
	{
		if (plastseq) {
			plastseq->InsertFrame(pframe);
		}
	}

	ns_HoLin::sFrame* sFrames::SearchFrames(std::string_view s)
	{
		ns_HoLin::sSequenceOfFrames *pseq = pfirstseq;
		ns_HoLin::sFrame *p_frame = nullptr;

		while (pseq) {
			p_frame = pseq->Search(s);
			if (p_frame)
				return p_frame;
			pseq = pseq->pnextseq;
		}
		return (ns_HoLin::sFrame*)nullptr;
	}

	BOOL sFrames::FindFrames(std::string_view s)
	{
		ns_HoLin::sSequenceOfFrames *pseq = pfirstseq;

		while (pseq) {
			if (pseq->Find(s))
				return TRUE;
			pseq = pseq->pnextseq;
		}
		return FALSE;
	}

	ns_HoLin::sSequenceOfFrames* sFrames::SearchSequence(std::string_view s)
	{
		ns_HoLin::sSequenceOfFrames *pseq = pfirstseq;

		while (pseq) {
			if (pseq->pfirstframe) {
				if (pseq->pfirstframe->name == s)
					return pseq;
			}
			pseq = pseq->pnextseq;
		}
		return (ns_HoLin::sSequenceOfFrames*)nullptr;
	}

	BOOL sFrames::FindSequence(std::string_view s)
	{
		ns_HoLin::sSequenceOfFrames *pseq = pfirstseq;

		while (pseq) {
			if (pseq->Find(s))
				return TRUE;
			pseq = pseq->pnextseq;
		}
		return FALSE;
	}
}
