#include "main.h"

int nCount;
unsigned char _nKEY[128];
CHECK_t auth[8];
AUTH_t auth2;

void parse_neighborList(unsigned char* data){
	int i = 0, j = 0;
	int neighborLen = -1;

	int dev_num = 0;
	int macLen = 0;
	int uuidLen = 0;
	int keyLen = 0;
	int nCount = 0;

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

		macLen = data[i++] << 8;
		macLen |= data[i++] << 0;
		printf("macLen:%d \n",macLen);
		_mac = (unsigned char*)malloc(macLen);
		memset(_mac, 0, macLen);
		memcpy(_mac, &data[i], macLen);
		memcpy(auth[j].MAC, _mac, macLen);
		i += macLen;
/*	
		uuidLen = data[i++];
		printf("uuidLen:%d \n",uuidLen);
		_uuid = (unsigned char*) malloc (uuidLen);
		memset(_uuid, 0, uuidLen);
		memcpy(_uuid, &data[i], uuidLen);
		memcpy(auth[j].UUID, _uuid, uuidLen);
		i+= uuidLen;
*/
		keyLen = data[i++] << 8;;
		keyLen |= data[i++] << 0;
		printf("keyLen:%d \n",keyLen);
		_key = (unsigned char*) malloc (keyLen);
		memset(_key, 0, keyLen);
		memcpy(_key, &data[i], keyLen);
		memcpy(auth[j].nKEY, _key, keyLen);
		i+= keyLen;

		nCount = data[i++];
		
		printf("(%d)n[%02d] mac[%d]:%s key[%d]\n",nCount, dev_num, macLen,_mac, keyLen);
		//printf("Neighbor Dev:%02d mac[%d]:%s uuid[%d]:%s Key[%d]\n", dev_num, macLen,_mac, uuidLen,_uuid, keyLen);
		
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

void parse_enc(unsigned char* data){
	int i = 0;
	int macLen = 0;
	int encLen = 0;
	int len = 0;
	int keyLen = 0;
	int myMacLen = 0;
	int tagDataLen = 0;

	unsigned char* _enc = NULL;
	unsigned char* _MAC = NULL;
	unsigned char* _key = NULL;
	unsigned char* _tag = NULL;
	unsigned char* _dec = NULL;
	unsigned char* _tagData = NULL;

	unsigned char* myMac = NULL;

	printf("Parse enc\n");

	macLen = data[i++];
	_MAC = (unsigned char*) malloc(macLen);
	memcpy(_MAC, &data[i], macLen);
	i += macLen;

	myMac = get_bt_addr();
	myMacLen = strlen(myMac);

	if(myMacLen != macLen){
		printf("Mac Len failed\n");
	}else {
		printf("Mac len Correct\n");
		if(memcmp(myMac, _MAC, myMacLen) != 0){
			printf("MAC cmp failed\n");
		}else printf("MAC cmp correct\n");
	}

	encLen = data[i++] << 8;
	encLen |= data[i++] << 0;
	_enc = (unsigned char*) malloc(encLen);
	memcpy(_enc, &data[i], encLen);
	i += encLen;

	_key = (unsigned char*)malloc(128);
	aes_decrypt(_enc, _key, encLen, _nKEY, 128);

	printf("parse_enc key: ");
	printf_byte(_key, 128);

	tagDataLen = myMacLen + encLen;	
	_tagData = (unsigned char*)malloc(tagDataLen);

	i = 0;
	memcpy(&_tagData[i], myMac, myMacLen);
	i += myMacLen;
	memcpy(&_tagData[i], _enc, encLen);
	
	//_tag = HMAC_SHA512(_key, keyLen, _tagData, &len);
	_tag = HMAC_SHA1(_key, _tagData, &len);

	printf("_tag[%d]: ", len);
	printf_byte(_tag, len);

	sendTagPacket(_tag, len);

}

void parse_tag(unsigned char* data, int len, int index){
	int i = 0, j = 0, cnt = 0;
	int tagLen = 0;
	int tagDataLen = 0;
	int myTagLen = 0;
	int myMacLen = 0;
	int result = -1;

	unsigned char* _tag = NULL;
	unsigned char* _tagData = NULL;
	unsigned char* myTag = NULL;
	unsigned char* myMac = NULL;

	printf("Parse tag\n");

	tagLen = data[i++];
	_tag = (unsigned char*)malloc(tagLen);
	memcpy(_tag, &data[i], tagLen);
	i += tagLen;

	printf("_tag[%d]: ", tagLen);
	printf_byte(_tag, tagLen);

	myMacLen = strlen(auth[index].MAC);

	tagDataLen = myMacLen + 260;	
	_tagData = (unsigned char*)malloc(tagDataLen);

	i = 0;
	memcpy(&_tagData[i], auth[index].MAC, myMacLen);
	i += myMacLen;
	memcpy(&_tagData[i], auth[index].CIPHER, 260);

	myTag = HMAC_SHA1(auth[index].KEY, _tagData, &len);
	
	if(memcmp(_tag, myTag, myTagLen) != 0){
		printf("Auth Failed\n");
		auth[index].result = 1;
	}
	else {
		printf("Auth Success\n");
		auth[index].result = 2;
		auth2.cnt_authed++;
	}
	
	if(auth2.cnt_authed == auth2.cnt_auth){
		sendAuthResultPacket(1);
	}else{
		for(j = 0; j < auth2.cnt_neighbor; j++){
			if(auth[j].result > 0) cnt++;
		}
		if(cnt == auth2.cnt_neighbor)
			sendAuthResultPacket(0);
	}
	printf("NOW CHECK[%d] Authed:%d, Auth:%d, Cnt:%d\n",index, auth2.cnt_authed, auth2.cnt_auth, auth2.cnt_neighbor);
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
		printf("Parse nListPacket\n");
		i++;

		neighborList = (unsigned char*)malloc(dataLen);
		memcpy(neighborList, &data[i], dataLen);

		parse_neighborList(neighborList);

		//printf_byte(neighborList, dataLen + 1);

	}else if(data[i] == 0x03){
		//Authenticate
		printf("Parse authenticationPacket\n");
		i++;

		auth = (unsigned char*)malloc(dataLen);
		memcpy(auth, &data[i], dataLen);

		parse_authentication(auth);

	}else if(data[i] == 0x04){
		//neighbor Reigster
		printf("Parse nRegPacket\n");
		i++;
		neighbor = (unsigned char*)malloc(dataLen);
		memcpy(neighbor, &data[i], dataLen);

		parse_neighborRegister(neighbor);
	}else if(data[i] == 0x05){
		//Key
		printf("Parse keyPacket\n");
		i++;
		keyData = (unsigned char*)malloc(dataLen);
		memcpy(keyData, &data[i], dataLen);

		parse_recvKey(keyData, dataLen);
	}else if(data[i] == 0x06){
		//encryption
		printf("Parse encPacket\n");
		i++;
		encData = (unsigned char*) malloc(dataLen);
		memcpy(encData, &data[i], dataLen);
		parse_enc(encData);

		//sendTag(encData);

	}else if(data[i] == 0x07){
		//tag
		printf("Parse tagPacket\n");
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
		printf("Parse tagPacket\n");
		i++;
		tagData = (unsigned char*) malloc(dataLen);
		memcpy(tagData, &data[i], dataLen);
		parse_tag(tagData, dataLen, socket);

		printf("Result Time: %d \n",clock());
	}
}
