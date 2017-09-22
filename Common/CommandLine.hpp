#pragma once

#include <iostream>
#include <string>
#include <vector>
// #include <algorithm> // std::erase, ...

#define _WIN32_WINNT_WIN10 0x0502 // For CopyFile2. Defined before windows.h include.
#include <windows.h>

#include "Utility.hpp"
// #include "Dialog.hpp"
#include "Tools.hpp"

struct CmdArguments {
	static CmdArguments Initialize( const std::vector<std::wstring>& arguments ) {
		const std::wstring path( arguments[0] );
		std::wstring program_name;
		std::wstring program_directory;

		// TODO:: Test this: Removed file name from path->
		{
			const auto split_index = path.find_last_of( L"\\" );
			program_directory = path.substr( 0, split_index );
			program_directory = Path::InitializeSafely( program_directory ).path_;

			program_name = path.substr( split_index + 1 );
		} // <-
		
		if ( arguments.size() == 1 ) return { program_name, program_directory, L"", {} };

		std::wstring command( arguments[1] );
		ToLower( command );
		
		std::vector<std::wstring> args;
		for ( int i=2; i<arguments.size(); ++i ) {
			args.push_back( arguments[i] );
		}

		return CmdArguments{ program_name, program_directory, command, args };
	}

	static CmdArguments Initialize() {
		int argc;
		wchar_t** argv = CommandLineToArgvW( GetCommandLine(), &argc );

		std::vector<std::wstring> arguments( argc );

		for ( int i=0; i<argc; ++i ) {
			arguments[i] = std::wstring( argv[i] );
		}

		return CmdArguments::Initialize( arguments );
	}
	
	const std::wstring program_name_;
	const std::wstring program_directory_;
	const std::wstring command_;
	const std::vector<std::wstring> args_;
};