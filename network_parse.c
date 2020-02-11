#include "main.h"

int nCount;

void parse_neighborList(unsigned char* data){
	int i = 0, j = 0;
	int neighborLen = -1;

	int dev_num = 0;
	int macLen = 0;
	int uuidLen = 0;
	int keyLen = 0;

	unsigned char* _mac = NULL;
	unsigned char* _uuid = NULL;
	unsigned char* _key = NULL;

	printf("Parse neighborList\n");

	neighborLen = data[i++];
	if(neighborLen < 0) printf("Parse neighborListLen Error!!!\n");
	
	printf("neighborLen: %d \n", neighborLen);
	
	auth2.cnt_neighbor = neighborLen;
	//auth2.cnt_auth = neighborLen;
	auth2.cnt_auth = 1;

	for(j = 0; j < neighborLen; j++){
		dev_num = data[i++];
		printf("dev_num:%d \n",dev_num);
		auth[j].devId = dev_num;

		macLen = data[i++];
		printf("macLen:%d \n",macLen);
		_mac = (unsigned char*)malloc(macLen);
		memset(_mac, 0, macLen);
		memcpy(_mac, &data[i], macLen);
		memcpy(auth[j].MAC, _mac, macLen);
		i += macLen;
	
		uuidLen = data[i++];
		printf("uuidLen:%d \n",uuidLen);
		_uuid = (unsigned char*) malloc (uuidLen);
		memset(_uuid, 0, uuidLen);
		memcpy(_uuid, &data[i], uuidLen);
		memcpy(auth[j].UUID, _uuid, uuidLen);
		i+= uuidLen;

		keyLen = data[i++];
		printf("keyLen:%d \n",keyLen);
		_key = (unsigned char*) malloc (keyLen);
		memset(_key, 0, keyLen);
		memcpy(_key, &data[i], keyLen);
		memcpy(auth[j].nKEY, _key, keyLen);
		i+= keyLen;
		
		printf("Neighbor Dev:%02d mac[%d]:%s uuid[%d]:%s Key[%d]\n", dev_num, macLen,_mac, uuidLen,_uuid, keyLen);
		
	}
	
}

void parse_authentication(unsigned char* data){
	printf("Parse Authentication\n");
	//search_sdp_r();
	register_service();
}

void parse_neighborRegister(unsigned char* data){
	int i = 0;
	int devNum = -1;
	int macLen = -1;
	int uuidLen = -1;
	int keyLen = 128;

	unsigned char* _mac = NULL;
	unsigned char* _uuid = NULL;
	unsigned char* _key = NULL;

	printf("Parse neighborRegister: ");
	devNum = data[i++];

	macLen = data[i++];
	_mac = (char *)malloc(macLen);
	memcpy(_mac, &data[i], macLen);
	i += macLen;

	uuidLen = data[i++];
	_uuid = (char *)malloc(uuidLen);
	memcpy(_uuid, &data[i], uuidLen);
	i += uuidLen;

	printf("[%02d] mac[%d]:%s, uuid[%d]:%s\n", devNum, macLen, _mac, uuidLen, _uuid);
	
	_key = genSymKey(keyLen);
	nCount++;
	saveNeighborList(_key, keyLen, _mac, macLen, devNum);

	sendKeyPacket(_key, keyLen);

}

void parse_recvKey(unsigned char* data, int keyLen){

	int i = 0;
	unsigned char* _key;

	printf("Parse recvKey: %d\n",keyLen);
	//keyLen = data[i++];
	_key = (char *)malloc(keyLen);
	memcpy(_key, &data[i], keyLen);
	i += keyLen;

	//printf("key[%d]: %s\n",keyLen, _key);

	saveSymKey(_key, keyLen);
}


void parse_role(unsigned char* data){

	int i = 0, j = 0;
	int dataLen = -1;
	int neighborLen = -1;
	int num = -1;
	unsigned char* neighbor = NULL;
	unsigned char* neighborList = NULL;
	unsigned char* auth = NULL;
	unsigned char* keyData = NULL;
	unsigned char* encData = NULL;
	unsigned char* tagData = NULL;

	printf("Parse MD Role Init \n");

	if(data[i++] != 0x02) printf("Parse MD Header Error!!!\n");
	dataLen = data[i++] << 8;
	dataLen |= data[i++] << 0;

	if(dataLen < 0) printf("Parse MD DataLen Error!!!\n");
	if(data[i] == 0x02){
		//Setup
		i++;

		neighborList = (unsigned char*)malloc(dataLen +1);
		memcpy(neighborList, &data[i], dataLen +1);

		//parse_neighborList(neighborList);

		//printf_byte(neighborList, dataLen + 1);

	}else if(data[i] == 0x03){
		//Authenticate
		i++;

		auth = (unsigned char*)malloc(dataLen);
		memcpy(auth, &data[i], dataLen);

		parse_authentication(auth);

	}else if(data[i] == 0x04){
		//neighbor Reigster
		i++;
		neighbor = (unsigned char*)malloc(dataLen);
		memcpy(neighbor, &data[i], dataLen);

		parse_neighborRegister(neighbor);
	}else if(data[i] == 0x05){
		//Key
		i++;
		keyData = (unsigned char*)malloc(dataLen);
		memcpy(keyData, &data[i], dataLen);

		parse_recvKey(keyData, dataLen);
	}else if(data[i] == 0x06){
		//encryption
		i++;
		encData = (unsigned char*) malloc(dataLen);
		memcpy(encData, &data[i], dataLen);

		//sendTag(encData);

	}else if(data[i] == 0x07){
		//tag
		i++;
		tagData = (unsigned char*) malloc(dataLen);
		memcpy(tagData, &data[i], dataLen);
		//checkTag(tagData, dataLen, 0);

		printf("Result Time: %d \n",clock());
	}else if(data[i] == 0x08){
		//Auth2
		printf("Result Time: %d \n",clock());
	}

	//free(neighbor);

}

void parse_role_socket(unsigned char* data, int socket){
	int i = 0;
	int dataLen = -1;
	unsigned char* tagData = NULL;

	printf("Parse MD Role Socket Init \n");

	if(data[i++] != 0x02) printf("Parse MD Header Error!!!\n");
	dataLen = data[i++] << 8;
	dataLen |= data[i++] << 0;

	if(dataLen < 0) printf("Parse MD DataLen Error!!!\n");

	if(data[i] == 0x07){
		//tag
		i++;
		tagData = (unsigned char*) malloc(dataLen);
		memcpy(tagData, &data[i], dataLen);
		//checkTag(tagData, dataLen, socket);

		printf("Result Time: %d \n",clock());
	}
}
