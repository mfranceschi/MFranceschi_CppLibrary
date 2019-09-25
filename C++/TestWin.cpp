// MANUAL TEST FILE ONLY FOR WINDOWS
#ifdef _WIN32

/* Instructions for memory leak check */
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <Windows.h>
#include <iostream>
#include <string>
#include "Shlwapi.h"
#include "Toolbox.hpp"
#include "File.hpp"
#include <io.h>
#include <fcntl.h>
#include <ctime>
#include <cassert>
#include "Date.hpp"

using namespace std;

static const size_t NBR_ITER = 10*1000;
static const wchar_t* file_L = L"D:\\Vikings.scx";
static const char* file_s = "D:\\Vikings.scx";
#define FUNC Toolbox::Timethis(NBR_ITER, [&](void) mutable
static long long reteval;

static void timingTimethis()
{
	cout << "The duration of the execution of 'Timethis' itself, without anything to do, is: " << FUNC{
	}) << endl;
	cout << endl;
}

static void timingTheFileExistence()
{
	cout << "Timing the file existence functions !" << endl;
	

	cout << "Time for PathFileExists: " << FUNC {
		reteval = PathFileExists(file_L);
	}) << endl;

	cout << "Time for stat: " << FUNC{
		struct stat d;
		reteval = !stat(file_s, &d);
	}) << endl;

	cout << "Time for GetFileAttributes: " << FUNC{
		DWORD attr = GetFileAttributes(file_L);
		reteval = (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY));
	}) << endl;

	cout << endl;
}

static void timingTheFileSize()
{
	cout << "Timing the file size functions !" << endl;

	cout << "Time for fseek : " << FUNC {
		FILE* f = fopen(file_s, "r");
		fseek(f, 0, SEEK_END);
		reteval = ftell(f);
		fclose(f);
	}) << endl;

	cout << "Time for stat: " << FUNC{
		struct stat st;
		stat(file_s, &st);
		reteval = st.st_size;
	}) << endl;

	cout << "Time for GetFileAttributesEx: " << FUNC{
		WIN32_FILE_ATTRIBUTE_DATA fileInfo;
		GetFileAttributesEx(file_L, GetFileExInfoStandard, (void*)& fileInfo);
		reteval = fileInfo.nFileSizeLow;
	}) << endl;

	cout << "Time for GetFileSizeEx: " << FUNC {
		HANDLE file = CreateFile(file_L, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		LARGE_INTEGER res;
		GetFileSizeEx(file, &res);
		reteval = res.QuadPart;
		CloseHandle(file);
	}) << endl;

	cout << endl;
}

static void timingWchar_tConversion()
{
	cout << "Timing char to wchar_t conversion functions!" << endl;
	const wchar_t* newptr = nullptr;

	cout << "Time for mbstowcs_s: " << FUNC{
		newptr = Toolbox::ToWchar_t(file_s);
	}) << endl;
	delete[] newptr;

	cout << "Time for wstring creation: " << FUNC{
		std::string s = file_s;
		newptr = std::wstring(s.cbegin(), s.cend()).c_str();
	}) << endl;

	cout << endl;
}

static void timingFileReading()
{
	cout << "Timing functions for reading 5 chars in a file!" << endl;
	char* buffer = new char[6];

	cout << "Time for FILE* with fgets: " << FUNC{
		FILE * file = fopen(file_s, "r");
		fgets(buffer, 6, file);
		fclose(file);
	}) << endl;

	cout << "Time for FILE* with fgetc: " << FUNC{
		FILE * file = fopen(file_s, "r");
		for (int i = 0; i < 5; ++i)
			buffer[i] = fgetc(file);
		buffer[5] = '\0';
		fclose(file);
	}) << endl;

	cout << "Time for ifstream with getline: " << FUNC{
		std::ifstream ifs(file_s);
		ifs.getline(buffer, 6);
		ifs.close();
	}) << endl;

	cout << "Time for ifstream with get: " << FUNC{
		std::ifstream ifs(file_s);
		for (int i = 0; i < 5; ++i)
			buffer[i] = ifs.get();
		buffer[5] = '\0';
		ifs.close();
	}) << endl;

	cout << "Time for Windows syscalls: " << FUNC{
		int file;
		_sopen_s(&file, file_s, _O_RDONLY, _SH_DENYNO, _S_IREAD);
		auto tmp = _read(file, buffer, 5);
		_close(file);
	}) << endl;

	cout << endl;
}

static void timingCtimeFunctions()
{
	cout << "Timing <ctime> conversion functions between tm and time_t !" << endl;
	time_t timet = time(nullptr);
	tm tmt;
	
	cout << "Time for localtime: " << FUNC{
		tmt = *localtime(&timet);
	}) << endl;

	cout << "Time for localtime_s: " << FUNC{
		localtime_s(&tmt, &timet);
	}) << endl;

	cout << "Time for mktime (with local [FR] time tm): " << FUNC{
		timet = mktime(&tmt);
	}) << endl;

	cout << "Time for gmtime: " << FUNC{
		tmt = *gmtime(&timet);
	}) << endl;

	cout << "Time for gmtime_s: " << FUNC{
		gmtime_s(&tmt, &timet);
	}) << endl;

	cout << "Time for mktime (with GMT time tm): " << FUNC{
		timet = mktime(&tmt);
	}) << endl;

	cout << endl;
}

void CheckReadMemoryLeaks()
{
	_CrtMemState states[3];
	_CrtMemCheckpoint(&states[0]);

	for (int i = 0; i < 25; ++i)
	{
		auto size = File::Size(file_L);
		const char* content = File::Read(file_L);
		Toolbox::InCharArrayStream isac(content, size);
		std::string strbuf;
		while (isac) getline(isac, strbuf);
		File::Read_Close(content);
	}
	_CrtMemCheckpoint(&states[1]);
	if (_CrtMemDifference(&states[2], &states[0], &states[1]))
		_CrtMemDumpStatistics(&states[2]);
}

void DoAllTimings() {
	timingTimethis();
	timingTheFileExistence();
	timingTheFileSize();
	timingWchar_tConversion();
	timingFileReading();
	timingCtimeFunctions();
	CheckReadMemoryLeaks();
}

int main()
{
	int x;
	DoAllTimings();
	cin >> x;
	return EXIT_SUCCESS;
}

#endif // _WIN32

