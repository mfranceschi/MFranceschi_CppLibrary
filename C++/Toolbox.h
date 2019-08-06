/* Martin Franceschi - MyWorks package - Toolbox module.
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
	double Timethis(size_t iter, const std::function<void(void)>&);

	// Returns the sign of the given number: -1, 0 or +1, as an int.
	template <typename type = long long>
	inline constexpr int Sign(type nbr)
	{
		return !nbr ?
			0 : nbr < type(0) ?
			-1 :
			+1;
	}

	// A constexpr function for computing a power of 2.
	constexpr unsigned long long constexpr_pow2(unsigned int nbr)
	{
		return 1ull << nbr;
	}

	// A constexpr function for computing min or max of two integers.
	// Returns first parameter in case of equality.
	template <typename Ta>
	constexpr Ta constexpr_minmax(Ta a, Ta b, bool min)
	{
		if (min)
			return a <= b ? a : b;
		else
			return a >= b ? a : b;
	}
}
#endif // TOOLBOX_H 
