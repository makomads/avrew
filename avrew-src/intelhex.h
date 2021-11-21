#ifndef __INTELHEX__
#define __INTELHEX__


long HexToBin(std::string &hex, unsigned char* bin, int maxbinsize);
int BinToHex(unsigned char *bin, int fsize, std::string &hex);


int loadIntelHex(char *file, unsigned char* image, int maxsize);
int saveIntelHex(char *file, unsigned char* image, int size);

#endif


