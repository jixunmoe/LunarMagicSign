int loadFile(const char *filename, unsigned char **result);
unsigned long searchCriticalCode (char *blockToSearch, long maxLen);

void printHex (unsigned char *blockToSearch, unsigned int start, unsigned int len);
void printUnsignedInt (unsigned int nNumber);


unsigned char sign_odd  (unsigned char charToSign);
unsigned char sign_even (unsigned char charToSign);

unsigned char sign_odd_r  (unsigned char charToSign);
unsigned char sign_even_r (unsigned char charToSign);

unsigned int sign_is_pos_odd ( unsigned int nPos );

unsigned int signBlock (unsigned char *blockToSign, unsigned long start, unsigned long len, unsigned int initalChecksum);
void calcExtraBit (unsigned int hashOrigional, unsigned int hashAgainst, unsigned char **extraBit);
