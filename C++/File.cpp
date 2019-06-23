//---------- Implementation of module <File> (file File.cpp) 

//--------------------------------------------------------------- Includes
#define _CRT_SECURE_NO_WARNINGS // FILE* operations are considered unsafe.
#pragma warning( disable: 26444) // Disables a warn' that occurs when using imbue.

#include <Windows.h>
#include <algorithm>
#include <locale>
#include <codecvt>
#include <sys/stat.h>
#include "File.h"
#include "../Globals.h"

using std::string; 
using std::locale; 
using std::ifstream; 
using std::ios_base;

namespace SystemIO
{
	namespace File
	{
/////////////////////////////////////////////////////////////////  PRIVATE
		namespace
		{
//-------------------------------------------------------------- Constants

			// Locale for UTF-16LE encoding.
			const locale locUTF8;
			const locale locUTF16LE(
				locale(),
				new std::codecvt_utf16<char, 0x10ffff, std::little_endian>
			); 
//------------------------------------------------------------------ Types

//------------------------------------------------------- Static variables

//------------------------------------------------------ Private functions
			
			// Returns true if file exists.
			// Inputs: any filename.
			bool exists(const string& filename)
			{
				struct stat d;
				return !stat(filename.c_str(), &d);
			}

			// Returns false if file is less than 4 chars long.
			// Inputs: any filename.
			// Warnings: throws a FileException if file does not exist 
			// or cannot be read.
			bool isEmpty(const string& filename)
			{
				if (!exists(filename))
				{
					throw S_Exception(Err::FILENOEXISTS, filename.c_str());
				}
				
				FILE* f = fopen(filename.c_str(), "r");
				if (f == nullptr)
				{
					throw S_Exception(Err::FILEOPEN, filename.c_str());
				}

				bool forReturn;
				fgetc(f);
				fgetc(f);
				fgetc(f);
				fgetc(f);
				forReturn = feof(f) + ferror(f);
				fclose(f);
				return forReturn;
			}
		}

//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Public functions

		void Open(ifstream& ifs, const string& filename)
		{
			ifs.close();
			string encoding = File::Encoding(filename);

			if (encoding.compare(ENC_UTF8) == 0)
			{
				ifs.open(filename);
				ifs.imbue(locUTF8);
				ifs.seekg(3);
				return;
			}
			else if (encoding.compare(ENC_UTF16LE) == 0)
			{
				ifs.open(filename, ios_base::binary);
				ifs.imbue(locUTF16LE);
				ifs.seekg(2, ios_base::beg);
				return;
			}
			else if (encoding.compare(ENC_DEFAULT) == 0)
			{
				ifs.open(filename, ios_base::binary);
				return;
			}
			else {
				throw S_Exception(Err::UNKNOWN, filename.c_str());
			}
		}

		filesize_t Size(const string& filename)
		{
			if (!exists(filename))
				throw S_Exception(Err::FILENOEXISTS, filename.c_str()); 
			
			// Converting file name to wstring (unsafe way but we think that 
			std::wstring wfilename (filename.cbegin(), filename.cend());

			// Now we use Windows WIN32 API tools.
			WIN32_FILE_ATTRIBUTE_DATA fileInfo;
			if (!GetFileAttributesEx(wfilename.c_str(), GetFileExInfoStandard, (void*)& fileInfo))
				throw S_Exception(Err::FILEOPEN, filename.c_str()); // Failed to get file infos.
			return fileInfo.nFileSizeLow;
			
			/* I decided not to use the usual (below) because 
			 * it may create a wrong size value on a big file. */
			
			/*FILE* f = fopen(filename.c_str(), "r");
			if (f == nullptr) 
				throw S_Exception(Err::FILEOPEN, filename.c_str());

			fseek(f, 0, SEEK_END);
			size_t taille = ftell(f);
			fclose(f);
			return taille;*/
		}

		string Encoding(const string& filename)
		{
			if (isEmpty(filename))
				throw S_Exception(Err::FILEEMPTY, filename.c_str());

			FILE* f = fopen(filename.c_str(), "r");

			int char1 = fgetc(f);
			int char2 = fgetc(f);
			int char3 = fgetc(f);

			string forReturn;
			if (char1 == 0xff && char2 == 0xfe)
				forReturn = ENC_UTF16LE;
			else if (char1 == 0xef && char2 == 0xbb && char3 == 0xbf)
				forReturn = ENC_UTF8;
			else
				forReturn = ENC_DEFAULT;

			fclose(f);
			return forReturn;
		}
	}
} 
