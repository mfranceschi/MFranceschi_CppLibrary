// MANUAL TEST FILE ONLY FOR UNIX SYSTEMS
#ifndef _WIN32

#include <climits>
#include <cstdlib>
#include "File.hpp"
#include <iostream>
#include <streambuf>
#include <sstream>
#include <string>
#include "Toolbox.hpp"

using namespace std;

void myassert(bool condition, const char* str, int nbr = -1) 
{
	if (!condition)
	{
		if (nbr == -1)
			cout << str << endl;
		else
			cout << str << " : " << nbr << endl;
		exit(EXIT_FAILURE);
	}
}

int main()
{
	int iter = 99;
	File::filename_t fname = "bonjour.txt";
	int nbr_lignes = 1;

	if (false)
	{
		for    (int j=0 ; j<iter ; ++j)
			for(int i=0 ; i<iter ; ++i)
			{
				const char* content = File::Read(fname);
				myassert(content != nullptr, "fichier pas ok");
				Toolbox::InCharArrayStream icas(content, File::Size(fname));
				std::string buffer;
				getline(icas, buffer);
				int linecpt = 0;
				while (icas) {
					getline(icas, buffer);
					++linecpt;
				}
				myassert(linecpt == nbr_lignes, "BaD LiNe CoUnT", linecpt);
				
				File::Read_Close(content);
			}
	}

	if(true) 
	{
		const char* content = File::Read(fname);
		myassert(content != nullptr, "fichier pas ok");
		Toolbox::InCharArrayStream icas(content, File::Size(fname));
		std::string buffer;
		getline(icas, buffer);
		int linecpt = 0;
		while (icas) {
			getline(icas, buffer);
			++linecpt;
			cout << buffer << endl;
		}
		myassert(linecpt == nbr_lignes, "BaD LiNe CoUnT", linecpt);

		File::Read_Close(content);
	}

	if(false) 
	{
		for(int i=0;i<iter;++i) {
			for (int j=0;j<iter;++j) {
				istringstream iss("bonjour");
				iss.get();
			}
		}
	}
	
	cout << "FAIT" << endl;
	return EXIT_SUCCESS;
}

#endif
