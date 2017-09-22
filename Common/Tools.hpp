#pragma once

#include <string>
#include <vector>

#include "Common/Utility.hpp"
// #include "Dialog.hpp"

// Main ->
enum class PathType {
	kDirectory,
	kFile,
};

// void BuildPath( const PathType type, std::wstring& path );

PathType ToPathType( const std::wstring& path );
// PathType PathToPathType( const std::wstring& path );

struct Path {
	struct InvalidPathException : std::runtime_error {
		explicit InvalidPathException(	std::string exception )
									:	std::runtime_error( exception )
		{}
	};

	static Path Initialize( std::wstring path ) {
		CleanPath( path );
		return Path{ path, ToPathType( path ) };
	}
	static Path InitializeSafely( std::wstring path ) {
		if ( !FindAndCleanPath( path ) ) {
			
			std::string error( "Path: " );
			error += std::string( std::begin( path ), std::end( path ) );
			error += " not found.";

			throw InvalidPathException{ error };
		}
		return Path{ path, ToPathType( path ) };
	}
	
	Path( const std::wstring& path, const PathType type )
		: path_( path ), type_( type ) {}

	bool IsDirectory() const { return type_ == PathType::kDirectory; }
	bool IsFile() const { return type_ == PathType::kFile; }
	
	const std::wstring path_;
	const PathType type_;
};


enum class OpenOperation {
	kDefault,
	kOpen,
	kEdit,
	kExplore, // TODO:: Fix this to not use Explorer!
};

std::wstring OpenOperationToString( const OpenOperation open_operation );

bool Open(	const Path& file,
			const std::wstring& open_operation = L"",
			const std::wstring& parameters = L"" );

bool Open(	const Path& file,
			const OpenOperation open_operation,
			const std::wstring& parameters = L"" );

bool Delete( const Path& file );
bool Move( const Path& from, const Path& to );
bool Copy( const Path& from, const Path& to );
bool Rename( const Path& old_name, const Path& new_name );
bool Exists( const Path& path );

std::vector<std::wstring> FilesInDirectory( const std::wstring& directory );