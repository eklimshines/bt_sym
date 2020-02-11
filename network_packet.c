#include "main.h"

CHECK_t auth[8];
AUTH_t auth2;
extern int nCount;

void sendnListPacket(){
	unsigned char* data = NULL;
	unsigned char* _nData = NULL;

	int i = 0, j = 0;
	int macLen = 0;
	int uuidLen = 0;
	int keyLen = 0;
	int dataLen = 0;
	int htLen = 5;
	int nDataLen = 0;

	printf("sendnListPacket(Con->Main)[%d]\n",nCount);

	_nData = readNeighborList(&nDataLen);

	dataLen = nDataLen + 1;
	data = (unsigned char *)malloc(dataLen + htLen);
	
	data[i++] = 0x02;
	data[i++] = dataLen >> 8;
	data[i++] = dataLen >> 0;
	data[i++] = 0x02;

	data[i++] = nCount;

	memcpy(&data[i], _nData, nDataLen);
	i += nDataLen;

	data[i++] = 0x03;
	data[i] = '\0';
	
	//printf_byte(data, dataLen + htLen);
        client_send(data, dataLen + htLen);

}

void sendAuthReqPacket(){
	unsigned char* data = NULL;

	int i = 0, j = 0;
	int dataLen = 1;
	int htLen = 5;

	printf("sendAuthReqPacket(Con->Main)\n");

	data = (unsigned char *)malloc(dataLen + htLen);

	data[i++] = 0x02;
	data[i++] = dataLen >> 8;
	data[i++] = dataLen >> 0;
	data[i++] = 0x03;
	data[i++] = 0x00;
	data[i++] = 0x03;
	data[i] = '\0';

        client_send(data, dataLen + htLen);
}

void sendnRegPacket(){
	unsigned char* data = NULL;
	char* mac = get_bt_addr();
	char* uuid = get_bt_uuid();
	int i = 0;
	int macLen = strlen(mac);
	int uuidLen = 0; //strlen(uuid);
	int dataLen = 3 + macLen + uuidLen;
	int htLen = 5;

	printf("sendnRegPacket(Neighbor->Con)\n");

	data = (unsigned char *)malloc(dataLen + htLen);

	data[i++] = 0x02;
	data[i++] = dataLen >> 8;
	data[i++] = dataLen >> 0;
	data[i++] = 0x04;
	data[i++] = DEV_ID; //0x05; //Rasp-5
	data[i++] = macLen;
	//MAC
	memcpy(&data[i], mac, macLen);
	i += macLen;

	//UUID
	data[i++] = uuidLen;
	memcpy(&data[i], uuid, uuidLen);
	i += uuidLen;
	
	data[i++] = 0x03;
	data[i] = '\0';
	//printf_byte(data, dataLen + headerTailLen);
        client_send(data, dataLen + htLen);

}

void sendKeyPacket(unsigned char* _key, int keyLen){
	unsigned char* data = NULL;
	int htLen = 5;
	int i = 0;

	printf("sendKeyPacket(Con->Neighbor)[%d]\n",keyLen);

	//_key = genSymKey(128, &keyLen);
	data = (unsigned char *)malloc(keyLen + htLen);

	data[i++] = 0x02;
	data[i++] = keyLen >> 8;
	data[i++] = keyLen >> 0;
	data[i++] = 0x05;
	
	memcpy(&data[i], _key, keyLen);
	i+= keyLen;
	data[i++] = 0x03;
	data[i] = '\0';
	
	//printf_byte(data, keyLen + htLen);

        server_tcpsend(data, keyLen + htLen);
}

void sendEncPakcet(char* ID, int index){
	unsigned char* _enc = NULL;
	unsigned char* data = NULL;
	
	int i = 0;
	int checkMac = -1;
	int encLen = -1;
	int macLen = strlen(ID);
	int dataLen = -1;
	clock_t start, end;

	start = clock();
	//checkMac = select_neighborByMAC(ID);
	
	if(checkMac <= 0){
		printf("No MAC \n");
		//return NULL;
	}else printf("MainDevice Send Enc Func\n");

	//_enc = _sk_kem_enc(ID, &encLen, index);

	memcpy(auth[index].CIPHER, _enc, encLen);

	dataLen = 10 + encLen + macLen;
	data = (unsigned char*) malloc(dataLen);
	
	data[i++] = 0x02;

	data[i++] = dataLen >> 8;
	data[i++] = dataLen >> 0;

	data[i++] = 0x06;

	data[i++] = macLen;
	memcpy(&data[i], ID, macLen);
	i += macLen;

	data[i++] = encLen >> 8;
	data[i++] = encLen >> 0;

	memcpy(&data[i], _enc, encLen);
	i += encLen;

	data[i++] = 0x03;
	data[i] = '\0';

	//blue_send(data, dataLen);
	blue_send_sock(data, dataLen,auth[index].socket);

	//l2cap_send_sock(data, dataLen, auth[index].socket);
	end = clock();
	printf("The elapse time : %fs\n", (double) (end - start)/CLOCKS_PER_SEC);

}

void sendTagPacket(unsigned char* _tag, int tagLen){
	int i = 0;
	int dataLen = 0;

	unsigned char* data;

	dataLen = 6 + tagLen;
	data = (unsigned char*) malloc(dataLen);

	data[i++] = 0x02;

	data[i++] = dataLen >> 8;
	data[i++] = dataLen >> 0;

	data[i++] = 0x07;

	data[i++] = tagLen;
	memcpy(&data[i], _tag, tagLen);
	i += tagLen;

	data[i++] = 0x03;

	l2cap_send(data, dataLen);
	//blue_send(data, dataLen);
}


void sendAuthResultPacket(int result){
	unsigned char* data = NULL;

	int i = 0;
	int dataLen = 0;
	int htLen = 5;

	printf("sendAuthResultPacket(Main->Con)\n");

	dataLen = 1;
	data = (unsigned char*)malloc(dataLen + htLen);

	data[i++] = 0x02;
	data[i++] = dataLen >> 8;
	data[i++] = dataLen >> 0;
	data[i++] = 0x08;

	data[i++] = result;

	data[i++] = 0x03;
	data[i] = '\0';

        server_tcpsend(data, dataLen + htLen);
}
