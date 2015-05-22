int loadFile(const char *filename, unsigned char **result);
unsigned long searchCriticalCode (char *blockToSearch, long maxLen);

void printHex (unsigned char *blockToSearch, unsigned int start, unsigned int len);
void printUnsignedInt (unsigned int nNumber);

unsigned char sign_0 (unsigned char i);
unsigned char sign_1 (unsigned char i);
unsigned char sign_2 (unsigned char i);

unsigned char sign_r_0 (unsigned char i);
unsigned char sign_r_1 (unsigned char i);
unsigned char sign_r_2 (unsigned char i);

unsigned int signBlock (
	unsigned char *blockToSign,
	
	unsigned long start,
	unsigned long len,
	
	unsigned long initPosition,
	unsigned int initalChecksum
);
unsigned int calcExtraBit (unsigned int hashOrigional, unsigned long nHashOffset, unsigned char **extraBit);
