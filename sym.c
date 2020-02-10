#include "main.h"

unsigned char* genSymKey(int keyLen, *int len){
	unsigned char symkey[keyLen];

	if (!RAND_bytes(symkey, keyLen)){
        	exit(-1);
	}
	symkey[keyLen-1] = '\0';

	return symKey;
}
