#include "Tools.hpp"

#include <map>

#include "../DirExplorer/DirExplorer/src/Dialog.hpp"


#include "Shlwapi.h"					// For PathIsDirectory
#pragma comment( lib, "Shlwapi.lib" )	// For PathIsDirectory

PathType ToPathType( const std::wstring& path ) {
	return ( PathIsDirectory( path.c_str() ) == FALSE ) ? PathType::kFile : PathType::kDirectory;
}

std::wstring OpenOperationToString( const OpenOperation open_operation ) {
	static const std::map<OpenOperation, std::wstring> operations_to_string = {
		{ OpenOperation::kEdit, L"edit" },
		{ OpenOperation::kOpen, L"open" },
		{ OpenOperation::kExplore, L"explore" },
		{ OpenOperation::kDefault, L"" },
	};
	return operations_to_string.at( open_operation );
}

bool Open(	const Path& file,
			const std::wstring& open_operation,
			const std::wstring& parameters )
{
	static bool is_initialized = false;
	if ( !is_initialized ) {
		CoInitializeEx( nullptr, COINIT_MULTITHREADED );
		is_initialized = true;
	}

	const auto instance = ShellExecute(
		nullptr,
		( open_operation != L"" ) ? open_operation.c_str() : nullptr,
		file.path_.c_str(),
		( parameters != L"" ) ? parameters.c_str() : nullptr,
		nullptr, // TODO:: default (working) directory= "If this value is NULL, the current working directory is used."
		SW_SHOWNORMAL
	);

	// return reinterpret_cast<int>( instance ) > 32; // "If the function succeeds, it returns a value greater than 32"
	return ( (int)( (HINSTANCE)instance ) ) > 32; // "If the function succeeds, it returns a value greater than 32"
}

bool Open(	const Path& file,
			const OpenOperation open_operation,
			const std::wstring& parameters )
{
	const std::wstring operation( OpenOperationToString( open_operation ) );
	return Open( file, operation, parameters );
}

bool Delete( const Path& file ) {
	SHFILEOPSTRUCT file_operation = {0};
		
	file_operation.wFunc = FO_DELETE;
	
	const auto z_file = std::wstring( file.path_ + L'\0' );
	file_operation.pFrom = z_file.c_str();

	file_operation.fFlags =	FOF_ALLOWUNDO;

	const auto result = SHFileOperation( &file_operation );

	if ( file_operation.fAnyOperationsAborted ) {
		std::wstring caption( L"Delete fail" );
		std::wstring text( L"Not all deletion operations were completed." );
		ShowErrorDialog( caption, text );
		return false;
	}
	return true;
}

bool Move( const Path& from, const Path& to ) {
	if ( to.type_ != PathType::kDirectory ) throw;

	SHFILEOPSTRUCT file_operation = {0};
		
	file_operation.wFunc = FO_MOVE;
	
	const auto z_from = std::wstring( from.path_ + L'\0' );
	const auto z_to = std::wstring( to.path_ + L'\0' );

	file_operation.pFrom = z_from.c_str();
	file_operation.pTo = z_to.c_str();

	file_operation.fFlags =	FOF_ALLOWUNDO |
							// FOF_SIMPLEPROGRESS |
							FOF_WANTNUKEWARNING;

	const auto result = SHFileOperation( &file_operation );

	if ( file_operation.fAnyOperationsAborted ) {
		std::wstring caption( L"Move fail" );
		std::wstring text( L"Not all move operations were completed." );
		ShowErrorDialog( caption, text );
		return false;
	}
	
	return true;
}


bool Copy( const Path& from, const Path& to ) {
	if ( to.type_ != PathType::kDirectory ) throw;


	SHFILEOPSTRUCT file_operation = {0};
		
	file_operation.wFunc = FO_COPY;
	
	const auto z_from = std::wstring( from.path_ + L'\0' );
	const auto z_to = std::wstring( to.path_ + L'\0' );

	file_operation.pFrom = z_from.c_str();
	file_operation.pTo = z_to.c_str();

	file_operation.fFlags =	FOF_ALLOWUNDO |
							FOF_SIMPLEPROGRESS |
							FOF_WANTNUKEWARNING;

	const auto result = SHFileOperation( &file_operation );

	if ( file_operation.fAnyOperationsAborted ) {
		std::wstring caption( L"Copy fail" );
		std::wstring text( L"Not all copy operations were completed." );
		ShowErrorDialog( caption, text );
		return false;
	}
	
	return true;
}

bool Rename( const Path& old_name, const Path& new_name ) {
	if (	old_name.type_ == PathType::kDirectory ||
			new_name.type_ == PathType::kDirectory )
	{
		throw;
	}

	SHFILEOPSTRUCT file_operation = {0};
	
	file_operation.wFunc = FO_RENAME;

	const auto z_from = std::wstring( old_name.path_ + L'\0' );
	const auto z_to = std::wstring( new_name.path_ + L'\0' );

	file_operation.pFrom = z_from.c_str();
	file_operation.pTo = z_to.c_str();

	file_operation.fFlags =	FOF_ALLOWUNDO;
		
	/*ShowErrorDialog( L"from", z_from.c_str() );
	ShowErrorDialog( L"to", z_to );*/

	const auto result = SHFileOperation( &file_operation );

	if ( file_operation.fAnyOperationsAborted ) {
		std::wstring caption( L"Rename fail" );
		std::wstring text( L"Rename operation failed." );
		ShowErrorDialog( caption, text );
		return false;
	}
	return true;
}

bool Exists( const Path& path ) {
	const auto result = CreateFile2(	path.path_.c_str(),
										GENERIC_READ,
										0,
										OPEN_EXISTING,
										nullptr );

	if ( result == INVALID_HANDLE_VALUE ) return false;

	CloseHandle( result );
	return true;
}


std::vector<std::wstring> FilesInDirectory( const std::wstring& directory ) {
	std::vector<std::wstring> files_;

	// First found item.
	WIN32_FIND_DATA initial_data = {0};

	HANDLE search_handle = FindFirstFile( directory.c_str(), &initial_data );
	// if ( search_handle == INVALID_HANDLE_VALUE ) return {};
	if ( search_handle == INVALID_HANDLE_VALUE ) return files_;

	files_.push_back( std::wstring( initial_data.cFileName ) );
	
	// Rest of the data.
	WIN32_FIND_DATA next_data = {0};

	while ( FindNextFile( search_handle, &next_data ) != 0 ) {
		files_.push_back( std::wstring( next_data.cFileName ) );
		next_data = {0};
	}


	// FindNextFile should fail (return 0) when there are no more files.
	if ( GetLastError() != ERROR_NO_MORE_FILES ) throw;

	
	const auto closed_succeeded = FindClose( search_handle );
	if ( closed_succeeded == 0 ) throw;

	return std::move( files_ );
}