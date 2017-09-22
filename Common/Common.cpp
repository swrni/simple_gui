#include "Common.hpp"

#include <iostream>
#include <algorithm> // std::remove_if, ...

void TrimString(	std::wstring& str,
					const std::wstring& cs_to_save )
{
	str.erase(
		std::remove_if(
			std::begin( str ),
			std::end( str ),
			[&cs_to_save]( const wchar_t& c ){ return cs_to_save.find( c ) == std::wstring::npos; }
		),
		std::end( str )
	);
}

#include "Shlwapi.h"					// For PathFileExists
#pragma comment( lib, "Shlwapi.lib" )	// For PathFileExists

void CleanPath( std::wstring& path ) {
	static const std::wstring alphabets( L"abcdefghijklmnopqrstuvwxyzåäö" );
	static const std::wstring c_alphabets( L"ABCDEFGHIJKLMNOPQRSTUVWXYZÅÄÖ" );
	static const std::wstring nums( L"0123456789" );
	static const std::wstring specials( L"+-_.:/\\ ()" );
	static const std::wstring characters = alphabets + c_alphabets + nums + specials;

	TrimString( path, characters );
}

bool FindPath( std::wstring& path ) {
	wchar_t buffer[ MAX_PATH+1 ] = {0};

	const auto chars_copied = GetFullPathName(
		path.c_str(),
		MAX_PATH,
		buffer,
		nullptr
	);

	if ( chars_copied == 0 ) return false;
	
	path = std::wstring( buffer, chars_copied );

	/*if ( PathIsRelative( path.c_str() ) == TRUE ) {
		wchar_t buffer[ MAX_PATH+1 ] = {0};

		const auto chars_copied = GetFullPathName(
			path.c_str(),
			MAX_PATH,
			buffer,
			nullptr
		);

		if ( chars_copied == 0 ) return false;
	
		path = std::wstring( buffer, chars_copied );
	}*/

	/*if ( PathFileExists( path.c_str() ) ) return true;
	
	wchar_t buffer[ MAX_PATH+1 ] = {0};
	const auto chars_copied = GetFullPathName(
		path.c_str(),
		MAX_PATH,
		buffer,
		nullptr
	);

	if ( chars_copied == 0 ) return false;
	
	path = std::wstring( buffer, chars_copied );*/

	return ( PathFileExists( path.c_str() ) == TRUE );
}

bool FindAndCleanPath( std::wstring& path ) {
	CleanPath( path );
	return FindPath( path );
}

void ToLower( std::wstring& str ) {
	std::transform( std::begin( str ), std::end( str ), std::begin( str ), ::tolower );
}

void PrintItems( const std::vector<std::wstring>& items ) {
	std::wcout << L"Item count: " << items.size() << L"\n";
	
	/*for ( const auto& item : items ) {
		std::wcout << item << L"\n";
	}*/
	for ( int i=0; i<items.size(); ++i ) {
		std::wcout << items[i] << L'\n';
	}
}

