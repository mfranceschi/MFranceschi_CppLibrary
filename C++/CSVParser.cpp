//---------- Implementation of class <CSVParser> (file CSVParser.cpp) 
#ifdef totoro_666
//---------------------------------------------------------------- INCLUDE

//-------------------------------------------------------- System includes
#include <cassert>
#include <sstream>

//------------------------------------------------------ Personal includes
#include "../Globals.h"
#include "CSVParser.h"
#include "File.h"
#include "../Business/Dates.h"
using namespace std;

namespace SystemIO
{
//-------------------------------------------------------------- Constants

//----------------------------------------------------------------- PUBLIC

//--------------------------------------------------------- Public methods
	
	void CSVParser::Clear()
	{
		theStream.clear();
	}

	vector<string> CSVParser::GetLine()
	{
		string line; // Line just read from the file.
		vector<string> res; // List of string (to be returned).
		
		getline(theStream, line); // Read the current line.
		line = sanitize(line);
		if (line.empty()) return res;
		
		istringstream iss(line);
		string temp;
		while (iss.good()) 
		{
			getline(iss, temp, separator);
			res.push_back(sanitize(temp));
		}
		
		// Remove empty strings at the end.
		while (res.back().empty())
			res.pop_back();
		
		return res;
	}

	streampos CSVParser::Goto(streampos pos)
	{
		// if utf16
		if ((enc == File::ENC_UTF16LE) && (pos % 2))
			pos = std::streamoff(pos) - 1;

		// If we want to go before the first character,
		// return the first character.
		if (pos <= firstPos)
		{
			if(theStream.tellg() != firstPos)
				theStream.seekg(streamoff(firstPos), ios_base::beg);
			return firstPos;
		}

		// If we want to go out of the file size, 
		// throw an error.
		if (pos >= size)
		{
			theStream.clear();
			throw S_Exception(Err::STREAMPOS);
		}

		theStream.seekg(pos);

		// Look for last '\n' before position.		
		int lastChar;
		while (((lastChar = theStream.peek()) != '\n') && State())
		{
			Clear();

			if (enc == File::ENC_UTF16LE)
				theStream.seekg(-4, ios_base::cur);
			else
				theStream.seekg(-1, ios_base::cur);


			// If we are at the beginning of the file, return it.
			if (theStream.tellg() <= firstPos)
			{
				theStream.seekg(firstPos);
				return firstPos;
			}
		}

		if (!State())
		{
			theStream.clear();
			theStream.seekg(firstPos);
			theStream.clear();
			throw S_Exception(Err::STREAMPOS);
		}

		// Return last '\n' + 1.
		theStream.get();
		return theStream.tellg();
	}

	streampos CSVParser::Move(streamoff offset)
	{
		if (enc == File::ENC_UTF16LE)
			offset = 2 * offset;
		return Goto(Pos() + offset);
	}

	void CSVParser::Open(const string& filename)
	{
		File::Open(theStream, filename);
		firstPos = theStream.tellg();
		size = File::Size(filename);
		enc = File::Encoding(filename);

		// Trying to detect separator
		vector<string> firstline = GetLine();
		if (
			firstline.size() == 1 && 
			count(firstline[0].cbegin(), firstline[0].cend(), ',') > 1)
			Separator(',');
		theStream.seekg(firstPos);
	}

	string CSVParser::Peek()
	{
		streampos current(theStream.tellg());
		string res;
		getline(theStream, res);
		size_t posOfSep = res.find_first_of(separator);
		theStream.seekg(current);
		res = (posOfSep == string::npos) ?
			res :
			res.substr(0, posOfSep);

		return sanitize(res);
	}

	streampos CSVParser::Pos()
	{
		return theStream.tellg();
	}

	streampos CSVParser::PreviousLine()
	{
		const streamoff currentpos = Pos();
		streamoff newpos;
		streamoff off = -1;
		do
		{
			newpos = Move(off);
			--off;
		} while (currentpos == newpos);
		return Pos();
	}

	char CSVParser::Separator(char newSep)
	{
		if (isprint(newSep))
		{
			separator = newSep;
		}
		return separator;
	}

	void CSVParser::Skip()
	{
		theStream.ignore(numeric_limits<streamsize>::max(), '\n');
	}

	bool CSVParser::State()
	{
		theStream.peek();
		return theStream.good() && theStream.is_open();
	}
//-------------------------------------------------- Operator overloadings

	CSVParser::operator bool()
	{
		return State();
	}

//---------------------------------------------- Constructors - destructor
	CSVParser::CSVParser() :
		separator (';'),
		theStream(),
		buffer (new char[Globals::BUF_SIZE]),
		firstPos(0),
		size(),
		enc()
	{
		theStream.rdbuf()->pubsetbuf(buffer, Globals::BUF_SIZE);
	} 

	CSVParser::~CSVParser ( )
	{
		delete[] buffer;
	} 


//---------------------------------------------------------------- PRIVATE

//------------------------------------------------------ Protected methods
	string CSVParser::sanitize(string& str)
	{
		while (!str.empty() && !isprint(str.back()))
			str.pop_back();
		return str;
	}
}
#endif
