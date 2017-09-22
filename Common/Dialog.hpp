#pragma once

#include <string>

enum class Command {
	kRetry,
	kCancel,
};

Command ShowRetryCancelDialog(	const std::wstring& caption,
								const std::wstring& text );

void ShowErrorDialog(	const std::wstring& caption,
						const std::wstring& text );