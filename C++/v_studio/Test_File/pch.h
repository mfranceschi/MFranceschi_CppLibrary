//
// pch.h
// Header for standard system include files.
//

#pragma once

#include "gtest/gtest.h"
#include "../../File.hpp"

// First settings : file names, (Win) memory leaks check.
#if 1

	// File name constants

#define MIDDLESIZE_RAW      Vikings.scx
#define UNEXISTING_RAW      unexisting._tut
#define SMALL_UTF16LE_RAW   Small_utf16le.txt
#define TEMP_RAW            I_AM_TEMP
#define _Make_Fname(raw) FNAME_PREFIX #raw

#ifdef _WIN32 // WIN32
#ifdef UNICODE
#define FNAME_PREFIX LR"path(..\..\..\Test_File\TestFiles\)path"
constexpr File::filename_t FNAME_MIDDLESIZE = _Make_Fname(MIDDLESIZE_RAW);
constexpr File::filename_t FNAME_UNEXISTING = _Make_Fname(UNEXISTING_RAW);
constexpr File::filename_t FNAME_SMALL_UTF16LE = _Make_Fname(SMALL_UTF16LE_RAW);
constexpr File::filename_t FNAME_TEMP = _Make_Fname(TEMP_RAW);
#else
#define FNAME_PREFIX R"path(..\TestFiles\)path"
constexpr File::filename_t FNAME_MIDDLESIZE = FNAME_PREFIX "Vikings.scx";
constexpr File::filename_t FNAME_UNEXISTING = FNAME_PREFIX "unexisting._tut ";
constexpr File::filename_t FNAME_SMALL_UTF16LE = FNAME_PREFIX "Small_utf16le.txt";
#endif

#define I_Want_Mem_Leaks
#else // POSIX
constexpr File::filename_t FNAME_MIDDLESIZE = "/mnt/d/Vikings.scx";
constexpr File::filename_t FNAME_UNEXISTING = "/mnt/c/unexisting._tut";
#endif

// Turn on Memory Leaks detection (Win32 only)
#ifdef I_Want_Mem_Leaks
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#endif

// Structure that holds file information.
struct file_info_data
{
	File::filesize_t size;
	File::filename_t name;
	char firstByte, lastByte;
	bool exists;
	File::encoding_t encoding;
	size_t offset;

	file_info_data(File::filesize_t fsize, File::filename_t fname, char fByte, char lByte, bool ex, File::encoding_t enc, size_t offset = 0) :
		size(fsize), name(fname), firstByte(fByte), lastByte(lByte), exists(ex), encoding(enc), offset(offset)
	{}
};

