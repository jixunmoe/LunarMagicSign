// Build with std=c99

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

int main(int argc, char *argv[]) {
	printf (
	    "Lunar Magic Signer By Jixun\n"
	    "Version 1.11[For LM 2.31], Use at your own risk.\n"
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
			return 2;
		}
	}
	
	printf ("Size of source: 0x%08X\n", fSrcSize);
	
	// Calculate new hash for origional program
	unsigned int hashOrigional = signBlock (fSrc, 0, fSrcSize, 0, 0x10);
	if (argc == 4) {
		/* The hash starts calculate where it ends last time. */
		hashOrigional = signBlock (fMud, 0, fMudSize, fSrcSize, hashOrigional);
	}
	unsigned int hashAgainst = ~hashOrigional;
	
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
	unsigned int nExtraBytesize = calcExtraBit (hashOrigional, fSrcSize + fMudSize, &extraBit);
	
	printf ("Extra bit: ");
	printHex (extraBit, 0, nExtraBytesize);
	printf ("\n");
	
	// If output is set
	if (argc > 2) {
		printf ("Write to file ...\n");
		FILE *fOut = fopen (argv[2], "wb");
		fwrite (fSrc, 1, fSrcSize, fOut);
		// If it has Mud file
		if (fMudSize) {
			fwrite (fMud, 1, fMudSize, fOut);
		}
		
		if (nExtraBytesize && nExtraBytesize > 8) {
			fwrite (extraBit + 8, 1, nExtraBytesize - 8, fOut);
		}
		fwrite (hashes,   1, 8, fOut);
		if (nExtraBytesize)
			fwrite (extraBit, 1, 8, fOut);
		fclose (fOut);
	}
	printf ("Done, enjoy~\n");
	
	return 0;
}


