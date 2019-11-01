#include "main.h"

void saveParamCona(unsigned char* P1, int P1Len, unsigned char* P2, int P2Len, unsigned char* s, int sLen){

	unsigned char* data;
	int dataLen = 0;
	int i = 0;

	dataLen = P1Len + P2Len + sLen + 3;

	data = (unsigned char*) malloc(dataLen);
	
	data[i++] = P1Len;
	memcpy(&data[i], P1, P1Len);
	i += P1Len;

	data[i++] = P2Len;
	memcpy(&data[i], P2, P2Len);
	i += P2Len;

	data[i++] = sLen;
	memcpy(&data[i], s, sLen);
	i += sLen;
	
	saveParamCon(data, dataLen);
}

void readParamCon(){
	FILE *fp;
	int ch;
	int i =0;
	unsigned char* data;
	unsigned char read[1024]= {0};
	// test.bin 이라는 이진파일을 읽기 전용으로 열기
  	if ( (fp = fopen("controller.bin", "rb")) == NULL ) {
    		fputs("파일 열기 에러!", stderr);
    		exit(1);
  	}
  	//이진 파일, 1바이트씩 읽어, 헥사로 출력
  	while ( (ch = fgetc(fp)) != EOF ) {
    		//printf("%02X ", ch);
		read[i] = ch;
		i++;
  	}
	//printf("\n");
	//printf_byte(read, i);
	
	setPublicParamController(read, i);
	//setPublicParamNeighbor(read, i);
	fclose(fp);
}

void saveParamCon(unsigned char* param, int len){

	FILE *fp;
	int i =0;

	fp = fopen("controller.bin", "wb");
	// 이진 파일에, array[] 배열의 요소를 하나씩 쓰기
  	for (i = 0; i < len; i++) {
    		if(fputc(param[i], fp) == EOF) {
      			fputs("파일 쓰기 에러!", stderr);
      			exit(2);
    		}
  	}
	fclose(fp);
}

void readParamNeighbor(){
	FILE *fp;
	int ch;
	int i =0;
	unsigned char* data;
	unsigned char read[1024]= {0};
	// test.bin 이라는 이진파일을 읽기 전용으로 열기
  	if ( (fp = fopen("neighbor.bin", "rb")) == NULL ) {
    		fputs("파일 열기 에러!", stderr);
    		exit(1);
  	}
  	//이진 파일, 1바이트씩 읽어, 헥사로 출력
  	while ( (ch = fgetc(fp)) != EOF ) {
    		//printf("%02X ", ch);
		read[i] = ch;
		i++;
  	}
	//printf("\n");
	//printf_byte(read, i);
	
	setPublicParamNeighbor(read, i);
	fclose(fp);
}

void saveParamNeighbor(unsigned char* param, int len){

	FILE *fp;
	int i =0;

	fp = fopen("neighbor.bin", "wb");
	// 이진 파일에, array[] 배열의 요소를 하나씩 쓰기
  	for (i = 0; i < len; i++) {
    		if(fputc(param[i], fp) == EOF) {
      			fputs("파일 쓰기 에러!", stderr);
      			exit(2);
    		}
  	}
	fclose(fp);
}

unsigned char* encData(element_t U, element_t V, element_t W){
	unsigned char *data = NULL;
	unsigned char *_U, *_V, *_W;
	int uLen, vLen, wLen;
	int dataLen = 0;
	int i = 0;

	//element_printf("__U =%B \n",U);
	//element_printf("__V =%B \n",V);
	//element_printf("__W =%B \n",W);

	uLen = element_length_in_bytes(U);
	vLen = element_length_in_bytes(V);
	wLen = element_length_in_bytes(W);

	_U = pbc_malloc(uLen);
	_V = pbc_malloc(vLen);
	_W = pbc_malloc(wLen);

	element_to_bytes(_U,U);
	element_to_bytes(_V,V);
	element_to_bytes(_W,W);

	dataLen = uLen + vLen + wLen + 12;

	//printf("d[%d], u[%d], v[%d], w[%d]\n",dataLen, uLen, vLen, wLen);

	data = (unsigned char*)malloc(dataLen);

	data[i++] = 0x00;
	data[i++] = 0x02;
	data[i++] = dataLen >> 8;
	data[i++] = dataLen >> 0;

	data[i++] = uLen >> 8;
	data[i++] = uLen >> 0;

	memcpy(&data[i],_U,uLen);


	//printf("uLen: ");
	//printf_byte(_U, uLen);
	//printf("strlen(_U): ");
	//printf_byte(_U, strlen(_U));

	i += uLen;

	data[i++] = vLen >> 8;
	data[i++] = vLen >> 0;

	memcpy(&data[i],_V,vLen);

	//printf_byte(_V, strlen(_V));

	i += vLen;

	data[i++] = wLen >> 8;
	data[i++] = wLen >> 0;

	memcpy(&data[i],_W,wLen);

	//printf_byte(_W, wLen);

	i += wLen;
	data[i++] = 0x00;
	data[i++] = 0x03;

	//printf_byte(data, dataLen);

	//element_from_bytes(temp1,_U);
	//element_printf("temp1 =%B \n",temp1);

	free(_U);
	free(_V);
	free(_W);

	return data;

}
