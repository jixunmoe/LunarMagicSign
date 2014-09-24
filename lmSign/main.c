// Build with std=c99

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "functions.h"

int main(int argc, char *argv[]) {
	printf (
	    "Lunar Magic Signer By Jixun\n"
	    "Version 1.0 Use at your own risk.\n"
		"\n"
	    "Lunar Magic was created by FuSoYa:\n"
	    "http://fusoya.eludevisibility.org/lm/\n"
		"\n"
	    "Usage: ./lmSign <input> <output> [mud]\n"
	    "\n\n"
	);
	
	if (argc < 3) {
		printf ("Error: no input nor output specified in argument.\n");
		return 1;
	}
	
	// printf ("Argument count: %d\n", argc);
	
	unsigned char *fSrc;
	long fSrcSize = 0;
	
	unsigned char *fMud;
	long fMudSize = 0;
	
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
	
	unsigned int criticalPos = searchCriticalCode (fSrc, fSrcSize);
	printf ("Target Address: 0x%08X\n", criticalPos);
	
	unsigned int posA = criticalPos + 7;
	unsigned int posB = posA    + 4 + 4;
	
	printf ("Hash A: ");
	printHex (fSrc, posA, 4);
	printf ("\n");
	
	printf ("Hash B: ");
	printHex (fSrc, posB, 4);
	printf ("\n");
	
	const char *nop = "\x4A\x31\x4A\x31";
	
	// Over write the nop to old path.
	memcpy (fSrc + posA, nop, 4);
	memcpy (fSrc + posB, nop, 4);
	
	// Calculate new hash
	unsigned int hashOrigional = signBlock (fSrc, 0, fSrcSize, 0);
	if (argc == 4) {
		hashOrigional = signBlock (fMud, 0, fMudSize, hashOrigional);
	}
	unsigned int hashAgainst   = ~0 - hashOrigional;
	
	printf ("Hash A: 0x%08X\n", hashOrigional);
	printf ("Hash B: 0x%08X\n", hashAgainst  );
	
	// Patch the values
	memcpy (fSrc + posA, &hashOrigional, 4);
	memcpy (fSrc + posB, &hashAgainst  , 4);
	
	unsigned char *extraBit;
	calcExtraBit (hashOrigional, hashAgainst, &extraBit);
	
	printf ("Extra bit: ");
	printHex (extraBit, 0, 8);
	printf ("\n");
	
	printf ("Write to file ...\n");
	FILE *fOut = fopen (argv[2], "wb");
	fwrite (fSrc, sizeof (char), fSrcSize, fOut);
	fwrite (extraBit, sizeof (char), sizeof (extraBit), fOut);
	if (argc == 4) {
		fwrite (fMud, sizeof (char), fMudSize, fOut);
	}
	fclose (fOut);
	printf ("Done, enjoy~\n");
	
	return 0;
}


