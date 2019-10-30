#include "main.h"

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
