// Build with std=c99

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long loadFile (const char *filename, unsigned char **result) { 
	long size = 0;
	
	FILE *f = fopen (filename, "rb");
	if (f == NULL) { 
		*result = NULL;
		return 0;
	} 
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);
	*result = (unsigned char *)malloc(size + 1);
	if (size != fread(*result, sizeof(char), size, f)) { 
		free(*result);
		return 0;
	} 
	fclose(f);
	(*result)[size] = 0;
	return size;
}

unsigned long searchCriticalCode (char *blockToSearch, long maxLen) {
	char* pos = blockToSearch;
	unsigned int i = 0;
	for (; i < maxLen; i++) {
		if (memcmp (pos, "\xb8\xfc\xff\xff\x33\xf6\xbf", 7) == 0)
			return i;
		
		pos ++;
	}
	
	return 0;
}

void printHex (unsigned char *blockToSearch, unsigned int start, unsigned int len) {
	unsigned char* pos = blockToSearch + start;
	
	for (unsigned int i = 0; i < len; i++, pos++)
		printf ("%02X ", *pos);
		
	return ;
}

void printUnsignedInt (unsigned int nNumber) {
	unsigned char z[sizeof nNumber];
	memcpy (z, &nNumber, sizeof nNumber);
	printHex (z, 0, sizeof nNumber);
}

unsigned char sign_0 (unsigned char i) {
	// r += __ROL__(i, 2) ^ 0x41;
	
	return ((i & 0x3f) << 2 | (i >> 6)) ^ 0x41;
}

unsigned char sign_1 (unsigned char i) {
	// r -= __ROL__(i, 4) ^ 0x18;
	
	return ((i & 0x0f) << 4 | (i >> 4)) ^ 0x18;
}

unsigned char sign_2 (unsigned char i) {
	// r += ((i >> 1) + (i << 7) - 2) ^ 0xEA
	
	return ((i >> 1) + (i << 7) - 2) ^ 0xEA;
}

unsigned char sign_r_0 (unsigned char i) {
	// r += __ROL__(i, 2) ^ 0x41;
	
	i = i ^ 0x41;
	return (((i & 0x03) << 6) | (i >> 2));
}

unsigned char sign_r_1 (unsigned char i) {
	// r -= __ROL__(i, 4) ^ 0x18;
	
	i = i ^ 0x18;
	return ((i & 0x0f) << 4 | (i >> 4));
}

unsigned char sign_r_2 (unsigned char i) {
	// r += ((i >> 1) + (i << 7) - 2) ^ 0xEA;
	
	i = (i ^ 0xEA) + 2;
	return (i << 1) + (i >> 7);
}

unsigned int signBlock (
	unsigned char *blockToSign,
	
	unsigned long start,
	unsigned long len,
	
	unsigned long initPosition,
	unsigned int initalChecksum
) {
	unsigned char* pos = blockToSign + start;
	unsigned int ret = initalChecksum;
	unsigned long posDigit = initPosition;
	
	for (unsigned int i = 0; i < len; i++) {
		if (posDigit & 2) {
			ret += sign_2(*pos);
		} else if (posDigit & 1) {
			ret -= sign_1(*pos);
		} else {
			ret += sign_0(*pos);
		}
		
		pos ++;
		posDigit ++;
	}
	
	return ret;
}

// Return the bytes to be written
unsigned int calcExtraBit (unsigned int hashOrigional, unsigned long nHashOffset, unsigned char **extraBit) {
	// **extraBit -> Address of pointer of extraBit
	//  *extraBit -> The pointer of extraBit
	*extraBit = (unsigned char *)malloc(8);
	unsigned int hashAgainst = ~hashOrigional;
	
	memcpy (*extraBit    , &hashOrigional, 4);
	memcpy (*extraBit + 4, &hashAgainst  , 4);
	
	int nOffsets = (int)signBlock (*extraBit, 0, 8, nHashOffset, 0);
	if (nOffsets == 0) {
		// That's lucky!
		return 0;
	}
	
	unsigned int remainder = abs(nOffsets) % 255;
	unsigned int i = 0;
	printf ("nOffset: %d\n", nOffsets);
	unsigned int t = (abs(nOffsets) / 255 + 1) << 2; // * 4
	free (*extraBit);
	*extraBit = (unsigned char *)malloc(t);
	unsigned char* pos = *extraBit;
	unsigned int tmp = 0;
	
	if (nOffsets > 0) {
		for (; i < t; i++, pos++, nHashOffset++) {
			// Fill all the Negatives
			if (nHashOffset & 2) {
				*pos = sign_r_2 (0);
			} else if (nHashOffset & 1) {
				// Negative
				*pos = sign_r_1 (nOffsets > 0xFF ? 0xFF : (nOffsets > 0 ? nOffsets : 0 ));
				nOffsets -= 0xFF;
			} else {
				*pos = sign_r_0 (0);
			}
		}
	} else {
		printf ("ErrorNotImplemented\n");
		return 0;
	}
	
	return t;
}


