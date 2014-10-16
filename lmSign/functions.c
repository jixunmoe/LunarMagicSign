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

unsigned char sign_odd (unsigned char i) {
	// ROL i, 2
	// 0x3f: 0011 1111
	return ((i & 0x3f) << 2 | (i >> 6)) ^ 0x29;
}

unsigned char sign_even (unsigned char i) {
	// ROL i, 4
	// 0x0f: 0000 1111
	return ((i & 0x0f) << 4 | (i >> 4)) ^ 0x28;
}

unsigned char sign_odd_r (unsigned char i) {
	// ROR i, 2
	// 0x03: 0000 0011
	i = i ^ 0x29;
	return (((i & 0x03) << 6) | (i >> 2));
}

unsigned char sign_even_r (unsigned char i) {
	// ROR i, 4  :: same as ROL i, 4
	// 0x0f: 0000 1111
	i = i ^ 0x28;
	return ((i & 0x0f) << 4 | (i >> 4));
}

unsigned int sign_is_pos_odd ( unsigned int nPos ) {
	return nPos & 2 ? 1 : 0;
}

unsigned int signBlock (unsigned char *blockToSign, unsigned long start, unsigned long len, unsigned int initalChecksum) {
	unsigned char* pos = blockToSign + start;
	unsigned int ret = initalChecksum;
	
	for (unsigned int i = 0; i < len; i++) {
		if (i & 2) {
			ret += sign_odd  (*pos);
		} else {
			ret -= sign_even (*pos);
		}
		pos ++;
		
		// printf ("%8X\n", ret);
	}
	
	return ret;
}

void calcExtraBit (unsigned int hashOrigional, unsigned int hashAgainst, unsigned char **extraBit) {
	// **extraBit -> Address of pointer of extraBit
	//  *extraBit -> The pointer of extraBit
	*extraBit = (unsigned char *)malloc(8);
	
	memcpy (*extraBit    , &hashOrigional, 4);
	memcpy (*extraBit + 4, &hashAgainst  , 4);
	
	unsigned char* pos = *extraBit;

	for (unsigned int i = 0; i < 8; i++) {
		if (!(i & 2)) {
			*pos = sign_even_r (sign_odd (*pos));
		} else {
			*pos = sign_odd_r (sign_even (*pos));
		}
		pos++;
	}
}


