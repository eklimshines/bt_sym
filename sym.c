#include "main.h"

unsigned char* genSymKey(int keyLen){
	unsigned char* symKey = NULL;

	symKey = (unsigned char*)malloc(keyLen);

	if (!RAND_bytes(symKey, keyLen)){
        	exit(-1);
	}
	symKey[keyLen-1] = '\0';

	return symKey;
}
