#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <iostream>
#include <string>
#include "Shlwapi.h"
#include "Toolbox.h"
#include "File.h"
#include <io.h>
#include <fcntl.h>
#include <ctime>
#include <cassert>
#include "Date.h"

using namespace std;

const size_t NBR_ITER = 10*1000;
const wchar_t* file_L = L"D:\\Vikings.scx";
const char* file_s = "D:\\Vikings.scx";
#define FUNC Toolbox::Timethis(NBR_ITER, [&](void) mutable
long long reteval;

void timingTimethis()
{
	cout << "The duration of the execution of 'Timethis' itself, without anything to do, is: " << FUNC{
	}) << endl;
	cout << endl;
}

void timingTheFileExistence()
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

void timingTheFileSize()
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

void timingWchar_tConversion()
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

void timingFileReading()
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

void timingCtimeFunctions()
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

int main()
{
	//timingTimethis();
	//timingTheFileExistence();
	//timingTheFileSize();
	//timingWchar_tConversion();
	//timingFileReading();
	//timingCtimeFunctions();
	auto bigfile = LR"(C:\Program Files (x86)\Overwatch\data\casc\data\data.018)";
	const char* content = File::Read(bigfile);
	auto size = File::Size(bigfile)/10;
	assert(content);
	cout << Toolbox::Timethis(1, [&](void) mutable {
		for (File::filesize_t i = 0;i < size;i++)
			void(0);
		}) << endl;
	File::Read_Close(content);
	
	return EXIT_SUCCESS;
}
