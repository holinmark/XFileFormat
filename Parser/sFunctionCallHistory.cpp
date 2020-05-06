#include "stdafx.h"

namespace ns_HoLin
{
	std::vector<std::string> sFunctionCallHistory::history;

	sFunctionCallHistory::sFunctionCallHistory(std::string currentfunction)
	{
		history.emplace_back(currentfunction);
	}

	sFunctionCallHistory::~sFunctionCallHistory()
	{
		if (history.size() > 0) {
			history.pop_back();
		}
	}

	void sFunctionCallHistory::Insert(std::string_view name)
	{
		history.emplace_back(name);
	}
	
	BOOL sFunctionCallHistory::PrintHistoryLog(BOOL ret)
	{
		std::clog << "\nFunction history log\n";
		for (auto it : history)
			std::clog << it << "()\n";
		history.clear();
		return ret;
	}
}
