/* Apokk - MyWorks package - Toolbox module.
 *
 * 
 */

 //---------- Interface of module <Toolbox> (file Toolbox.h) 
#if ! defined ( TOOLBOX_H )
#define TOOLBOX_H 

//--------------------------------------------------------------- Includes
#include <fstream>
#include <string>
#include <functional>

namespace Toolbox
{
	

	//-------------------------------------------------------------- Constants

	//////////////////////////////////////////////////////////////////  PUBLIC
	//------------------------------------------------------- Public functions

	// Returns the same C-string but converted in wchar_t*.
	// It is created using new[] --> please use delete[] after use!
	// Returns nullptr in case of failure.
	wchar_t* ToWchar_t(const char* source);

	// Returns the execution time for the given function.
	// It executes iter times and returns (total_time/iter).
	double Timethis(size_t iter, void(*)(void));
	double Timethis(size_t iter, const std::function<void(void)>&);

}
#endif // TOOLBOX_H 
