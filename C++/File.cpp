//---------- Implementation of module <File> (file File.cpp) 

//--------------------------------------------------------------- Includes
#define _CRT_SECURE_NO_WARNINGS // FILE* operations are considered unsafe.
#pragma warning( disable: 26444) // Disables a warn' that occurs when using imbue.

#include <Windows.h>
#include <algorithm>
#include <locale>
#include <codecvt>
#include "File.h"

using std::string; 
using std::locale; 
using std::ifstream; 
using std::ios_base;

namespace File
{
/////////////////////////////////////////////////////////////////  PRIVATE

//-------------------------------------------------------------- Constants

	const locale locUTF8(
		locale(), 
		new std::codecvt_utf8<char>());
	const locale locUTF16LE(
		locale(),
		new std::codecvt_utf16<char, 0x10ffff, std::little_endian>
	); 

//------------------------------------------------------------------ Types

//------------------------------------------------------- Static variables

//------------------------------------------------------ Private functions

//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Public functions

	bool Exists(const string& filename)
	{
		DWORD attr = GetFileAttributes(filename);
		return attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY);
	}

	bool IsEmpty(const string& filename, size_t charsToRead)
	{
		bool forReturn;

		FILE* f = fopen(filename.c_str(), "r");
		if (f == nullptr)
		{
			forReturn = true;
		}
		else
		{
			for (size_t i=0;i<charsToRead;++i)
			{
				fgetc(f);
			}
			forReturn = feof(f) + ferror(f);
			fclose(f);
		}

		return forReturn;
	}

	void Open(ifstream& ifs, const string& filename)
	{
		ifs.close();
		encoding_t encoding = File::Encoding(filename);

		if (encoding == ENC_UTF8)
		{
			ifs.open(filename);
			ifs.imbue(locUTF8);
			ifs.seekg(3);
		}
		else if (encoding == ENC_UTF16LE)
		{
			ifs.open(filename, ios_base::binary);
			ifs.imbue(locUTF16LE);
			ifs.seekg(2, ios_base::beg);
		}
		else if (encoding == ENC_DEFAULT)
		{
			ifs.open(filename, ios_base::binary);
		}
	}

	filesize_t Size(const string& filename)
	{
		std::wstring wfilename (filename.cbegin(), filename.cend());

		HANDLE file = CreateFile(wfilename, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		if (file == INVALID_HANDLE_VALUE) return -1;
		LARGE_INTEGER res;
		GetFileSizeEx(file, &res);
		return res.QuadPart;
	}

	encoding_t Encoding(const string& filename)
	{
		encoding_t forReturn;
		if (IsEmpty(filename, 3))
			forReturn = "0";
		else
		{
			FILE* f = fopen(filename.c_str(), "r");
			fseek(f, 0, SEEK_SET);

			int char1 = fgetc(f);
			int char2 = fgetc(f);
			int char3 = fgetc(f);

			
			if (char1 == 0xff && char2 == 0xfe)
				forReturn = ENC_UTF16LE;
			else if (char1 == 0xef && char2 == 0xbb && char3 == 0xbf)
				forReturn = ENC_UTF8;
			else
				forReturn = ENC_DEFAULT;

			fclose(f);
		}
		return forReturn;
	}
} 
