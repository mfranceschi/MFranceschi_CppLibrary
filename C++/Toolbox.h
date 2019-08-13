/* Martin Franceschi - MyWorks package - Toolbox module.
 *
 * 
 */

 //---------- Interface of module <Toolbox> (file Toolbox.h) 
#if ! defined ( TOOLBOX_H )
#define TOOLBOX_H 

//--------------------------------------------------------------- Includes
#include <climits>
#include <functional>
#include <sstream>
#include <string>

namespace Toolbox
{
	//------------------------------------------------------------------ Types

	// Simple "istringstream" extension for reading C-string
	// without copy and without end character.
	class InCharArrayStream : public std::istringstream
	{
	public:
		// InCharArrayStream() = default;
		// The full string and the size.
		// Reading content[size-1] is ok, reading content[size] is not.
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

	// A constexpr function for computing min or max of two integers.
	// Returns first parameter in case of equality.
	template <typename Ta>
	constexpr Ta constexpr_minmax(Ta a, Ta b, bool min = true)
	{
		if (min)
			return a <= b ? a : b;
		else
			return a >= b ? a : b;
	}

	// Returns the number of seconds allowed in this machine.
	// It depends on "sizeof(time_t)".
	constexpr int constexpr_max_years()
	{
		typedef unsigned long long ULL_t;
		ULL_t seconds_in_year = ULL_t(86400.L * 365.2425L);
		ULL_t possible_seconds = 1ull << ULL_t(ULL_t(sizeof(time_t)) * 8ull - 1ull);

		return int(Toolbox::constexpr_minmax(
			possible_seconds / seconds_in_year,
			ULL_t(INT_MAX), true));
	}

}
#endif // TOOLBOX_H 
