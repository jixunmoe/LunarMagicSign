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

void printHex (unsigned char *blockToSearch, unsigned long start, unsigned long len) {
	unsigned char* pos = blockToSearch + start;
	
	for (unsigned int i = 0; i < len; i++, pos++)
		printf ("%02X ", *pos);
		
	return ;
}

unsigned char sign_odd (unsigned char i) {
	return (i << 0x2 | i >> 0x6) ^ 0x29;
}

unsigned char sign_even (unsigned char i) {
	return (i << 0x5 | i >> 0x3) ^ 0x26;
}

unsigned char sign_odd_r (unsigned char i) {
	i ^= 0x29;
	return (i << 0x6 | i >> 0x2);
}

unsigned char sign_even_r (unsigned char i) {
	i ^= 0x26;
	return (i << 0x3 | i >> 0x5);
}

unsigned int signBlock (unsigned char *blockToSign, unsigned long start, unsigned long len, unsigned int initalChecksum) {
	unsigned char* pos = blockToSign + start;
	unsigned int ret = initalChecksum;
	
	for (unsigned int i = 0; i < len; i++) {
		if (i & 1) {
			ret += sign_odd  (*pos);
		} else {
			ret -= sign_even (*pos);
		}
		pos ++;
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
		if (!(i & 1)) {
			*pos = sign_even_r (sign_odd (*pos));
		} else {
			*pos = sign_odd_r (sign_even (*pos));
		}
		pos++;
	}
}


