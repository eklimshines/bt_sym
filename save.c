#include "main.h"

void readNeighborList(){
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
	
	fclose(fp);
}

void saveNeighborList(unsigned char* data, int len){
	FILE *fp;
	int i =0;

	fp = fopen("neighborList.bin", "wb");
	// 이진 파일에, array[] 배열의 요소를 하나씩 쓰기
  	for (i = 0; i < len; i++) {
    		if(fputc(data[i], fp) == EOF) {
      			fputs("파일 쓰기 에러!", stderr);
      			exit(2);
    		}
  	}
	fclose(fp);
}
