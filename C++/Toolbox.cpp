//---------- Implementation of module <Toolbox> (file Toolbox.cpp) 

//--------------------------------------------------------------- Includes

#include <Windows.h>
#include <chrono>
#include <cstring>
#include "Toolbox.h"

namespace Toolbox
{
	/////////////////////////////////////////////////////////////////  PRIVATE

	//-------------------------------------------------------------- Constants

	//------------------------------------------------------------------ Types

	//------------------------------------------------------- Static variables

	//------------------------------------------------------ Private functions

	//////////////////////////////////////////////////////////////////  PUBLIC
	//------------------------------------------------------- Public functions

	
	wchar_t* ToWchar_t(const char* source)
	{
		size_t length = strlen(source);
		wchar_t* destination = new wchar_t[length + 1];
		size_t retValue = 0;
		mbstowcs_s(&retValue, destination, length + 1, source, length);
		//(&retValue, destination, length + 1, &source, length, nullptr);
		if (!retValue)
		{
			delete[] destination;
			return nullptr;
		}
		else
		{
			return destination;
		}
	}

	double Timethis(size_t iter, void(*func) (void))
	{
		using namespace std::chrono;
		high_resolution_clock::time_point beggining = high_resolution_clock::now();
		for (size_t i = 0; i < iter; i++)
		{
			func();
		}
		return (duration<double>(high_resolution_clock::now() - beggining).count()) / double(iter);
	}

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
}
