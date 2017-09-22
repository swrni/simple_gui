#include "Dialog.hpp"

#include <iostream>
// #include <string>

#include <windows.h>

Command ShowRetryCancelDialog(	const std::wstring& caption,
								const std::wstring& text )
{

	const int type = MB_RETRYCANCEL; // | MB_ICONASTERISK;

	const auto result = MessageBoxEx(	nullptr,
										text.c_str(),
										caption.c_str(),
										type,
										0 );

	if ( result == IDRETRY ) return Command::kRetry;
	else if ( result == IDCANCEL ) return Command::kCancel;
	else throw;
}

void ShowErrorDialog(	const std::wstring& caption,
						const std::wstring& text )
{
	const int type = MB_OK | MB_ICONHAND;
	MessageBoxEx( nullptr, text.c_str(), caption.c_str(), type, 0 );
}