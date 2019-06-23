//---------- Implementation of class <BusinessFile> (file BusinessFile.cpp) 

//---------------------------------------------------------------- INCLUDE

//-------------------------------------------------------- System includes

//------------------------------------------------------ Personal includes
#include "../Globals.h"
#include "BusinessFile.h"
using namespace std;

namespace SystemIO
{
//-------------------------------------------------------------- Constants

//----------------------------------------------------------------- PUBLIC

//--------------------------------------------------------- Public methods
	void BusinessFile::Open(const std::string& filename)
	{
		parser.Open(filename);
		vector<string> firstLineOfFile = parser.GetLine();

		bool alright = true;
		if (firstLineOfFile.size() == firstLine.size()) {
			for (int i = 0; i < numOfSeparators; i++)
			{
				if (firstLineOfFile[i].compare(firstLine[i])) {
					alright = false;
				}
			}
		}
		else
		{
			alright = false;
		}

		if (!alright)
		{
			throw S_Exception(Err::WRONGFILE, filename.c_str());
		}
	}
	
	char BusinessFile::Separator(char newSep)
	{
		return parser.Separator(newSep);
	}

//-------------------------------------------------- Operator overloadings

//---------------------------------------------- Constructors - destructor
	BusinessFile::BusinessFile(short num) :
		parser(),
		firstLine(),
		numOfSeparators(num)
	{
		firstLine.reserve(num);
	} 


	BusinessFile::~BusinessFile ( )
	{
	} 


//---------------------------------------------------------------- PRIVATE

//------------------------------------------------------ Protected methods
}
