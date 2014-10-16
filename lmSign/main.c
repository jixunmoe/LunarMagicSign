// Build with std=c99

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

int main(int argc, char *argv[]) {
	printf (
	    "Lunar Magic Signer By Jixun\n"
	    "Version 1.1[For LM 2.30], Use at your own risk.\n"
		"\n"
	    "Lunar Magic was created by FuSoYa:\n"
	    "http://fusoya.eludevisibility.org/lm/\n"
		"\n"
	    "Usage: ./lmSign <input> <output> [mud]\n"
	    "\n\n"
	);
	
	if (argc < 2) {
		printf ("Error: no input nor output specified in argument.\n");
		return 1;
	}
	
	// printf ("Argument count: %d\n", argc);
	
	unsigned char *fSrc;
	unsigned int fSrcSize = 0;
	
	unsigned char *fMud;
	unsigned int fMudSize = 0;
	
	printf ("Target file: %s\n", argv[1]);
	printf ("Output file: %s\n", argv[2]);
	
	if (!(fSrcSize = loadFile ( argv[1], &fSrc ))) {
		printf ("Read input %s failed!", argv[1]);
		return 1;
	}
	if (argc == 4) {
		printf ("   Mud file: %s\n", argv[3]);
		
		if (!(fMudSize = loadFile ( argv[3], &fMud ))) {
			printf ("Read mud %s failed!", argv[3]);
			return 1;
		}
	}
	
	printf ("Size of source: 0x%08X\n", fSrcSize);
	
	// Calculate new hash
	unsigned int hashOrigional = signBlock (fSrc, 0, fSrcSize, 0);
	if (argc == 4) {
		hashOrigional = signBlock (fMud, 0, fMudSize, hashOrigional);
	}
	unsigned int hashAgainst   = ~0 - hashOrigional;
	
	unsigned char hashes[8];
	memcpy (hashes    , &hashOrigional, 4);
	memcpy (hashes + 4, &hashAgainst  , 4);
	
	printf ("Hash A: ");
	printHex (hashes, 0, 4);
	printf ("\n");
	
	printf ("Hash B: ");
	printHex (hashes, 4, 4);
	printf ("\n");
	
	unsigned char *extraBit;
	calcExtraBit (hashOrigional, hashAgainst, &extraBit);
	
	printf ("Extra bit: ");
	printHex (extraBit, 0, 8);
	printf ("\n");
	
	// If output is set
	if (argc > 2) {
		printf ("Write to file ...\n");
		FILE *fOut = fopen (argv[2], "wb");
		fwrite (fSrc, sizeof (char), fSrcSize, fOut);
		if (argc == 4) {
			fwrite (fMud, sizeof (char), fMudSize, fOut);
		}
		fwrite (hashes, sizeof (char), sizeof (extraBit), fOut);
		fwrite (extraBit, sizeof (char), sizeof (extraBit), fOut);
		fclose (fOut);
	}
	printf ("Done, enjoy~\n");
	
	return 0;
}


