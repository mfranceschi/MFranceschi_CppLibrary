/* Martin Franceschi - MyWorks package - Toolbox module.
 *
 * 
 */

 //---------- Interface of module <Toolbox> (file Toolbox.h) 
#if ! defined ( TOOLBOX_H )
#define TOOLBOX_H 

//--------------------------------------------------------------- Includes
#include <limits>
#include <functional>
#include <sstream>

/* Safety check (see https://stackoverflow.com/a/6884102). */
#ifdef max
#undef max
#endif // max

namespace Toolbox
{
	//------------------------------------------------------------------ Types

	// Simple "istringstream" extension for reading C-string
	// without copy and without end character.
	class InCharArrayStream : public std::istringstream
	{
	public:
		// The full string and the size.
		// Reading content[size-1] is ok, reading content[size] is undefined behavior.
		InCharArrayStream(const char* content, size_t size);

	protected:
		struct InCharArrayBuffer : public std::stringbuf
		{
			InCharArrayBuffer(const char* content, size_t size);
		};
		InCharArrayBuffer icab;
	};

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
	constexpr int Sign(type nbr)
	{
		return !nbr ?
			0 : nbr < type(0) ?
			-1 :
			+1;
	}

	// Returns the number of years allowed to be represented on this machine.
	// It depends on "sizeof(time_t)".
	constexpr int constexpr_max_years()
	{
#pragma warning( disable: 4554)
		typedef unsigned long long ULL_t;
		constexpr ULL_t seconds_in_year = ULL_t(60.0L * 60.0L * 24.0L * 365.2425L);
		constexpr ULL_t possible_seconds = std::numeric_limits<time_t>::max();
		constexpr ULL_t possible_years_with_big_int = possible_seconds / seconds_in_year;
		constexpr int max_year_with_struct_tm = std::numeric_limits<int>::max();

		if (possible_years_with_big_int > max_year_with_struct_tm)
			return max_year_with_struct_tm;
		else
			return int(possible_years_with_big_int);
#pragma warning(default: 4554)
	}

}
#endif // TOOLBOX_H 
