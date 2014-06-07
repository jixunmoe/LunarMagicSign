#include "ClsSignLM.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <iomanip>

using namespace std;

unsigned int ClsSignLM::searchCritical (unsigned int maxLen) {
	char* pos = outBuff;
	for (unsigned int i = 0; i < maxLen; i ++) {
		// cout << pos << endl;
		if (memcmp (pos, "\xb8\xfc\xff\xff\x33\xf6\xbf", 7) == 0)
			return i;
		
		pos ++;
		// "\xb8\xfc\xff\xff\x33\xf6\xbf"
	}
	
	return 0;
}

ClsSignLM::ClsSignLM(char* fileInput, char* fileOutput, char* fileMud) {
	mudSize = 0;
	srcSize = 0;
	initTable ();
	
	cout << "Patching:\t" << fileInput << endl;
	
	src.open (fileInput, std::ifstream::binary);
	if (!src || !src.is_open()) {
		cerr << "ERR: Can't read source file for reading!";
		error = 2;
		return ;
	}
	dst.open (fileOutput, ios::binary);
	if (!dst) {
		cerr << "ERR: Can't open output file for writing";
		error = 3;
		return ;
	}
	if (fileMud) {
		hMud.open (fileMud, ios::binary);
		if (hMud) {
			hMud.seekg (0, hMud.end);
			mudSize =  hMud.tellg();
			hMud.seekg (0, hMud.beg);

			mud = new char[mudSize];
			hMud.read (mud, mudSize);
			hMud.close ();
		}
	}
	
	// Read source file
	src.seekg (0, src.end);
	srcSize = src.tellg();
	src.seekg (0, src.beg);
	// Output buffer, src and mud will be used to calculate final hash.
	outBuff = new char [srcSize + mudSize + 8];
	src.read (outBuff, srcSize);
	src.close ();

	unsigned int criticalPos = searchCritical (srcSize);
	if (!criticalPos) {
		cerr << "ERR: Can't find critical code!";
		error = 4;
		return ;
	}
	const char * nop = "\x4A\x31\x4A\x31";
	
	 cout << showbase
		 << internal
		 << setfill('0')
		 << hex
		 
		 << "Position:\t" << setw(10) << criticalPos + 7
	     << ",  "         << setw(10) << criticalPos + 7 + 4 + 4
		 << endl;
	
	strncpy (&outBuff[criticalPos + 7]        , nop, 4);
	strncpy (&outBuff[criticalPos + 7 + 4 + 4], nop, 4);

	// Not sure how strcat works, I'll use strncpy instead.
	
	strncpy (&outBuff[srcSize], nop, 4);
	strncpy (&outBuff[srcSize + 4], nop, 4);
	if (hMud) strncpy (&outBuff[srcSize + 8], mud, mudSize);

	unsigned int finalHash = calc (srcSize + mudSize + 8);
	unsigned int hashPair  = (~0 - finalHash);
	
	cout << "Signature:\t" << setw(10) << finalHash
	     << ", ~"          << setw(10) << hashPair
		 << endl;
	
	// Patch memory
	memcpy (&outBuff[criticalPos + 7], &finalHash, 4);
	memcpy (&outBuff[criticalPos + 7 + 4 + 4], &hashPair, 4);
	
	// Used for generating file padding.
	char* filePad = new char [8];
	memcpy (&filePad[0], &finalHash, 4);
	memcpy (&filePad[4], &hashPair, 4);
	
	// Generate file padding to balance the hash.
	for (int i = 0; i < 4; i++) {
		outBuff[srcSize + 2 * i     ] = evenTable [odd (filePad [2 * i]    )];
		outBuff[srcSize + 2 * i + 1 ] = oddTable [even (filePad [2 * i + 1])];
	}
	
	cout << "File padding:\t" << filePad << endl;
	
	// Writes everything.
	dst.write(outBuff, srcSize + mudSize + 8);
}

unsigned int ClsSignLM::odd (unsigned char i) {
	return 0xFF & ((i << 0x2 | i >> 0x6) ^ 0x29);
}

unsigned int ClsSignLM::even (unsigned char i) {
	return 0xFF & ((i << 0x5 | i >> 0x3) ^ 0x26);
}

unsigned int ClsSignLM::calc (unsigned int length){
	unsigned int result = 0 ;
	
	for (unsigned int i = 0; i < length; i++) {
		if (i & 1) {
			result += odd  (outBuff[i]);
		} else {
			result -= even (outBuff[i]);
		}
	}
	
	return result ;
}

void ClsSignLM::initTable(){
	oddTable  = new char[0xFF];
	evenTable = new char[0xFF];
	
	for (short i = 0; i < 0xFF; i++) {
		oddTable  [odd (i)] = i;
		evenTable [even(i)] = i;
	}
}

ClsSignLM::~ClsSignLM() {
	if (src)  src.close ();
	if (dst)  dst.close ();
	if (hMud) hMud.close ();
		
	cout << "Finished." << endl;
}

