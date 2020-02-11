#include "main.h"

int nCount;
unsigned char _nKEY[128];

unsigned char* readNeighborList(int *len){
	FILE *fp;
	int ch;
	int i =0;
	unsigned char read[2048]= {0};

	printf("ReadNeighborList\n");

	// test.bin 이라는 이진파일을 읽기 전용으로 열기
  	if ( (fp = fopen("neighborList.bin", "rb")) == NULL ) {
    		fputs("파일 열기 에러!\n", stderr);
    		exit(1);
  	}
  	//이진 파일, 1바이트씩 읽어, 헥사로 출력
  	while ( (ch = fgetc(fp)) != EOF ) {
    		//printf("%02X ", ch);
		read[i] = ch;
		i++;
  	}
	
	fclose(fp);

	printf("last:%02x, %02x\n", ch, read[i-1]);

	printf_byte(read, i);
	*len = i;

	return read;
}

void saveNeighborList(unsigned char* _key, int keyLen, unsigned char* _mac, int macLen, int devNum){
	FILE *fp;
	
	unsigned char* data = NULL;

	int i =0;
	int len = keyLen + macLen + 6;

	printf("SaveNeighborList\n");
	data = (unsigned char*)malloc(len);

	data[i++] = devNum;

	data[i++] = macLen >> 8;
	data[i++] = macLen >> 0;
	memcpy(&data[i], _mac, macLen);
	i+= macLen;

	data[i++] = keyLen >> 8;
	data[i++] = keyLen >> 0;
	memcpy(&data[i], _key, keyLen);
	i+= keyLen;

	data[i++] = nCount;

	//fp = fopen("neighborList.bin", "wb");
	fp = fopen("neighborList.bin", "ab");
	// 이진 파일에, array[] 배열의 요소를 하나씩 쓰기
  	for (i = 0; i < len; i++) {
    		if(fputc(data[i], fp) == EOF) {
      			fputs("파일 쓰기 에러!", stderr);
      			exit(2);
    		}
  	}
	fclose(fp);
}


void readSymKey(){
	FILE *fp;
	int ch;
	int i =0;
	unsigned char* data;
	unsigned char read[1024]= {0};

	printf("ReadSymKey\n");

	// test.bin 이라는 이진파일을 읽기 전용으로 열기
  	if ( (fp = fopen("sym.bin", "rb")) == NULL ) {
    		fputs("파일 열기 에러!", stderr);
    		exit(1);
  	}
  	//이진 파일, 1바이트씩 읽어, 헥사로 출력
  	while ( (ch = fgetc(fp)) != EOF ) {
    		//printf("%02X ", ch);
		read[i] = ch;
		i++;
  	}
	
	memcpy(_nKEY, read, 128);
	printf("ReadSymKey[%d]\n",i);

	fclose(fp);
}

void saveSymKey(unsigned char* data, int len){
	FILE *fp;
	int i =0;

	printf("SaveSymKey\n");

	fp = fopen("sym.bin", "wb");
	// 이진 파일에, array[] 배열의 요소를 하나씩 쓰기
  	for (i = 0; i < len; i++) {
    		if(fputc(data[i], fp) == EOF) {
      			fputs("파일 쓰기 에러!", stderr);
      			exit(2);
    		}
  	}
	fclose(fp);
}
