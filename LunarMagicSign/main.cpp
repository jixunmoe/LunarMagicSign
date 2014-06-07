#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

#include "ClsSignLM.h"

using namespace std;
string ExtractFilename (const string& path) {
	return path.substr( path.find_last_of( pathSeparator ) + 1 );
}

int main (int argc, char **argv) {
	cout << "Lunar Magic Signer By Jixun" << endl
		 << " - Version 1.0, Use at your own risk" << endl << endl
		 << " Lunar Magic was created by FuSoYa:" << endl
		 << "  http://fusoya.eludevisibility.org/lm" << endl
		 << "Usage: ./" << ExtractFilename(argv[0]) << " <input> <output> [mud]" << endl << endl;

	if (argc < 3) {
		cerr << "ERR: Not enough arguments!" << endl;
		return 1;
	}

	ClsSignLM lmSigner (argv[1], argv[2], argv[3]);

	cout << endl;

	return lmSigner.error;
}
