int loadFile(const char *filename, unsigned char **result);
unsigned long searchCriticalCode (char *blockToSearch, long maxLen);
void printHex (char *blockToSearch, unsigned long start, unsigned long len);


unsigned char sign_odd  (unsigned char charToSign);
unsigned char sign_even (unsigned char charToSign);

unsigned char sign_odd_r  (unsigned char charToSign);
unsigned char sign_even_r (unsigned char charToSign);

unsigned int signBlock (unsigned char *blockToSign, unsigned long start, unsigned long len, unsigned int initalChecksum);
void calcExtraBit (unsigned int hashOrigional, unsigned int hashAgainst, unsigned char **extraBit);
