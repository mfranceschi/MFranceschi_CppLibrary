//---------- Implementation of module <Toolbox> (file Toolbox.cpp) 

//--------------------------------------------------------------- Includes

#include <chrono>
#include <cstring>
#include "Toolbox.h"

#ifdef _WIN32
#include <Windows.h>
#endif

namespace Toolbox
{
	/////////////////////////////////////////////////////////////////  PRIVATE

	//-------------------------------------------------------------- Constants

	//------------------------------------------------------------------ Types

	//------------------------------------------------------- Static variables

	//------------------------------------------------------ Private functions

	//////////////////////////////////////////////////////////////////  PUBLIC
	//------------------------------------------------------- Public functions

	
	double Timethis(size_t iter, const std::function<void(void)>& func)
	{
		using namespace std::chrono;
		high_resolution_clock::time_point beggining = high_resolution_clock::now();
		for (size_t i = 0; i < iter; i++)
		{
			func();
		}
		return (duration<double>(high_resolution_clock::now() - beggining).count()) / double(iter);
	}

#ifdef _WIN32
	wchar_t* ToWchar_t(const char* source)
	{
		size_t length = strlen(source);
		wchar_t* destination = new wchar_t[length + 1];
		size_t retValue = 0;
		mbstowcs_s(&retValue, destination, length + 1, source, length);
		if (!retValue)
		{
			delete[] destination;
			destination = nullptr;
		}
		return destination;
	}
#endif
}
