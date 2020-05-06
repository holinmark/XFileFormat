#include "stdafx.h"

namespace ns_HoLin
{
	sErrorMessageException::sErrorMessageException()
	{
		b_has_error_msg_outputed = FALSE;
		number_of_errors = 0;
	}
	
	sErrorMessageException::~sErrorMessageException()
	{
		b_has_error_msg_outputed = FALSE;
		number_of_errors = 0;
	}
	
	void sErrorMessageException::DisplayError(ns_HoLin::sFunctionCallHistory &functioncalls)
	{
		std::clog << error_messages << '\n';
		for (auto s : functioncalls.history) {
			std::clog << s;
		}
		functioncalls.history.clear();
		error_messages.clear();
		b_has_error_msg_outputed = TRUE;
	}
}
