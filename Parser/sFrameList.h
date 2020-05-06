#pragma once

#include <DirectXMath.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "sMeshList.h"

namespace ns_HoLin
{
	struct sFrame
	{
		std::string name;
		std::vector<std::string> mesh; // names of meshes associated with this frame
		std::vector<DirectX::XMFLOAT3> vectors;
		DirectX::XMFLOAT4X4 matrix;
		std::string parent_name;
		sFrame *pnextframe;
		static std::size_t number_of_frames_created;

		sFrame();
		~sFrame();
		void Cleanup(HANDLE = NULL);
	};

	struct sSequenceOfFrames
	{
		ns_HoLin::sFrame *pfirstframe, *plastframe;
		std::vector<std::string> framenames;
		std::unordered_map<std::string, std::vector<std::string>> parent_children;
		DWORD number_of_frames_in_sequence;
		sSequenceOfFrames *pnextseq;

		sSequenceOfFrames();
		~sSequenceOfFrames();
		void Cleanup(HANDLE = NULL);
		void InsertFrame(sFrame*);
		BOOL Find(std::string_view);
		sFrame* Search(std::string_view);
		sFrame* CreateNewFrame();
		sFrame* GetLastEntry();
	};

	struct sFrames
	{
		ns_HoLin::sSequenceOfFrames *pfirstseq, *plastseq;
		std::size_t number_of_sequences;

		sFrames();
		~sFrames();
		void Cleanup(HANDLE = NULL);
		ns_HoLin::sSequenceOfFrames* CreateNewSequence();
		ns_HoLin::sSequenceOfFrames* GetLastSequence() { return plastseq; }
		void InsertSequenceOfFrames(ns_HoLin::sSequenceOfFrames*);
		void InsertFrameIntoLastSequence(sFrame*);
		sFrame* SearchFrames(std::string_view);
		BOOL FindFrames(std::string_view);
		ns_HoLin::sSequenceOfFrames* SearchSequence(std::string_view);
		BOOL FindSequence(std::string_view);
	};
}
