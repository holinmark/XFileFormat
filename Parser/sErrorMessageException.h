#pragma once

#ifndef SERRORMESSAGEEXCEPTION_H
#define SERRORMESSAGEEXCEPTION_H

#include <Windows.h>
#include <string>
#include <exception>

namespace ns_HoLin
{
	struct sErrorMessageException : public std::exception
	{
		BOOL b_has_error_msg_outputed;
		DWORD number_of_errors;
		std::string error_messages;
		
		sErrorMessageException();
		~sErrorMessageException();
		void DisplayError(ns_HoLin::sFunctionCallHistory&);
	};
}

#endif
