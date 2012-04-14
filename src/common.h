#ifndef COMMON_H
#define COMMON_H

#include <Windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <vector>
#include <sstream>

#if defined(_MSC_VER) && defined(_DEBUG)
	#define _CRTDBG_MAP_ALLOC
	#ifdef _malloca
		#undef _malloca
	#endif
	#include <stdlib.h>
	#include <crtdbg.h>
	#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
	#ifdef realloc
		#undef realloc
	#endif
#endif

#endif
