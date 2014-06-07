#ifndef CLSSIGNLM_H
#define CLSSIGNLM_H

#include <iostream>
#include <fstream>
using namespace std;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	#define pathSeparator '\\'
#else
	#define pathSeparator '/'
#endif

class ClsSignLM {
		char* oddTable;
		char* evenTable;

		int mudSize, srcSize;
		ifstream src;
		ofstream dst;
		ifstream hMud;
		char* mud;
		char* outBuff;
		unsigned int calc (unsigned int length);
		void initTable ();
		unsigned int searchCritical(unsigned int maxLen);
		unsigned int odd (unsigned char i);
		unsigned int even (unsigned char i);
	public:
		ClsSignLM(char* fileInput, char* fileOutput, char* fileMud);
		~ClsSignLM();
		int error;
};

#endif // CLSSIGNLM_H
