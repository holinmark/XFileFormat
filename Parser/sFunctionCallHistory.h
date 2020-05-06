#pragma once

#include <Windows.h>
#include <string>

namespace ns_HoLin
{
	struct sFunctionCallHistory
	{
		static std::vector<std::string> history;

		sFunctionCallHistory() {}
		sFunctionCallHistory(std::string);
		~sFunctionCallHistory();
		
		static void Insert(std::string_view);
		static BOOL PrintHistoryLog(BOOL = FALSE);
	};
}
