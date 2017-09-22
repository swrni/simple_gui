#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#define SFML_PATH "C:\\SFML-2.4.1"
#define LIB "\\lib\\"

#define SFML_LIB_PATH SFML_PATH LIB

#ifdef STATIC_MODE
	#define STATIC "-s"
#else
	#define STATIC ""
#endif

#ifdef _DEBUG
	#define LIB_END "-d.lib"
#else
	#define LIB_END ".lib"
#endif



#pragma comment( lib, SFML_LIB_PATH "sfml-system" STATIC LIB_END )
#pragma comment( lib, SFML_LIB_PATH "sfml-window" STATIC LIB_END )
#pragma comment( lib, SFML_LIB_PATH "sfml-graphics" STATIC LIB_END )
#pragma comment( lib, SFML_LIB_PATH "sfml-audio" STATIC LIB_END )
#pragma comment( lib, SFML_LIB_PATH "sfml-network" STATIC LIB_END )
