#include "main.h"

CONFIG_t cfg;
char* get_bt_uuid(){

	printf("Get BT UUID[%d]: %s\n",strlen(cfg.myuuid), cfg.myuuid);

	return cfg.myuuid ;
}

char* get_bt_addr(){
	struct hci_dev_info di;
	int dev_id;
	static char addr[19] = { 0 };

	dev_id = hci_get_route(NULL);

	hci_devinfo(dev_id, &di);

	ba2str(&(di).bdaddr, addr);

	printf("Get BT ADDR[%d]: %s\n",strlen(addr), addr);
	return addr;
}

char* get_bt_name(){
	static struct hci_dev_info di;

	int dev_id;
	unsigned char name[248] = { 0 };

	dev_id = hci_get_route(NULL);

	hci_devinfo(dev_id, &di);

	printf("Get BT NAME[%d]: %s\n",strlen(di.name), di.name);
	return &di.name;

}

void printf_byte(unsigned char* data, int len){
	int i = 0;
	for (i = 0; i < len; i++){
		printf("%02x ",data[i]);
	}
	printf("\n");

}

void concate_byte(unsigned char* out, unsigned char* in1, int in1Len, unsigned char *in2, int in2Len){
	int i = 0;
	//printf("-1\n");
	for (i = 0; i <in1Len; i++){
		out[i] = in1[i];
	}
	//printf("0\n");
	for (i = 0; i <in2Len; i++){
		out[i+in1Len] = in2[i];
	}
	//printf_byte(out, in1Len+in2Len);
}

unsigned char* message_padding(unsigned char* data, int dataLen, int padding){

	int i = 0;
	unsigned char* temp =(unsigned char*)malloc(padding);
	strncpy(temp, data, dataLen);
	//*data = (unsigned char*)malloc(padding);
	//strncpy(data, temp, dataLen);

	for(int i = dataLen; i<padding; i++){
		temp[i]='0';
	}
	temp[padding] = '\0';
	printf("In Message[%d]: %s\n",strlen(temp),temp);

	return temp;
}

int _str2uuid(const char* uuid_str, uuid_t* uuid) {
    /* This is from the pybluez stack */

    uint32_t uuid_int[4];
    char* endptr;

    if (strlen(uuid_str) == 36) {
        char buf[9] = { 0 };

        if (uuid_str[8] != '-' && uuid_str[13] != '-' &&
            uuid_str[18] != '-' && uuid_str[23] != '-') {
            return 0;
        }
        // first 8-bytes
        strncpy(buf, uuid_str, 8);
        uuid_int[0] = htonl(strtoul(buf, &endptr, 16));
        if (endptr != buf + 8) return 0;
        // second 8-bytes
        strncpy(buf, uuid_str + 9, 4);
        strncpy(buf + 4, uuid_str + 14, 4);
        uuid_int[1] = htonl(strtoul(buf, &endptr, 16));
        if (endptr != buf + 8) return 0;

        // third 8-bytes
        strncpy(buf, uuid_str + 19, 4);
        strncpy(buf + 4, uuid_str + 24, 4);
        uuid_int[2] = htonl(strtoul(buf, &endptr, 16));
        if (endptr != buf + 8) return 0;

        // fourth 8-bytes
        strncpy(buf, uuid_str + 28, 8);
        uuid_int[3] = htonl(strtoul(buf, &endptr, 16));
        if (endptr != buf + 8) return 0;

        if (uuid != NULL) sdp_uuid128_create(uuid, uuid_int);
    }
    else if (strlen(uuid_str) == 8) {
        // 32-bit reserved UUID
        uint32_t i = strtoul(uuid_str, &endptr, 16);
        if (endptr != uuid_str + 8) return 0;
        if (uuid != NULL) sdp_uuid32_create(uuid, i);
    }
    else if (strlen(uuid_str) == 4) {
        // 16-bit reserved UUID
        int i = strtol(uuid_str, &endptr, 16);
        if (endptr != uuid_str + 4) return 0;
        if (uuid != NULL) sdp_uuid16_create(uuid, (uint16_t)i);
    }
    else {
        return 0;
    }

    return 1;
}

int readconfig(uint8_t *buf, int length, char *param, char *dst){
	int result = 0;
	int cmp = 0;
	int i = 0;
	int j = 0;

	for(i=0;i<length;i++){
		cmp = strncmp(&buf[i],param,strlen(param));
		if(cmp == 0){
			i = i+strlen(param)+1;
			//while((buf[i]==' ')||(buf[i]=='\t')||(buf[i]=='\n')||(buf[i]=='\r'))
			while((buf[i]==' ')||(buf[i]=='\t')){
				i++;
				if(i>length) break;
			}
			while(!((buf[i+j]=='\r')||(buf[i+j]=='\n')||(buf[i+j]==' ')||(buf[i+j]=='\t'))){
				if((i+j)>length) break;
				dst[j] = buf[i+j];
				j++;
			}
			break;
		}
	}

	result = i+j;
	
	return result;
}

void config_init(CONFIG_t *config){
	int fd = 0;
	int rcnt = 0;
	char buf[2048] = {0};
	char sbuf[2048] = {0};
	char* strmainip = "MAINDEVICEIP";
	char* strmainport = "MAINDEVICEPORT";
	char* strmainmac = "MAINDEVICEMAC";
	char* strmainuuid = "MAINDEVICEUUID";
	char* struserip = "USERIP";
	char* struserport = "USERPORT";
	char* strmyuuid = "MYUUID";
	char* strdev = "DEVNUM";
	int length = 0;
	int i = 0;

	char maindeviceip[16];
	char maindevicemac[20];
	char maindeviceuuid[40];
	char myuuid[40];
	char userip[20];

	int maindeviceport;
	int userport;
	int devnum;

	memset(config,0,sizeof(CONFIG_t));

	fd = open("./control.cfg",O_RDONLY|O_CREAT,0666);
	if(fd<=0){
		fd = open("/root/control/controlcfg",O_RDONLY|O_CREAT,0666);
	}
	if(fd>0){
		rcnt = read(fd, buf, 2048);
	}
	close(fd);

	memset(sbuf, 0, sizeof(sbuf));
	memset(config->maindeviceip, 0, sizeof(config->maindeviceip));
	length = readconfig(buf, rcnt, strmainip, sbuf);
	memcpy(config->maindeviceip, sbuf, strlen(sbuf));	

	memset(sbuf,0,sizeof(sbuf));
	memset(config->maindevicemac, 0, sizeof(config->maindevicemac));
	length = readconfig(buf, rcnt, strmainmac, sbuf);
	memcpy(config->maindevicemac, sbuf, strlen(sbuf));

	memset(sbuf,0,sizeof(sbuf));
	memset(config->maindeviceuuid, 0, sizeof(config->maindeviceuuid));
	length = readconfig(buf, rcnt, strmainuuid, sbuf);
	memcpy(config->maindeviceuuid, sbuf, strlen(sbuf));

	memset(sbuf,0,sizeof(sbuf));
	memset(config->myuuid, 0, sizeof(config->myuuid));
	length = readconfig(buf, rcnt, strmyuuid, sbuf);
	memcpy(config->myuuid, sbuf, strlen(sbuf));

	memset(sbuf,0,sizeof(sbuf));
	memset(config->userip, 0, sizeof(config->userip));
	length = readconfig(buf, rcnt, struserip, sbuf);
	memcpy(config->userip, sbuf, strlen(sbuf));

	memset(sbuf, 0, sizeof(sbuf));
	length = readconfig(buf, rcnt, strmainport, sbuf);
	config->maindeviceport = strtoul(sbuf, 0, 10);

	memset(sbuf, 0, sizeof(sbuf));
	length = readconfig(buf, rcnt, struserport, sbuf);
	config->userport = strtoul(sbuf, 0, 10);
	
	memset(sbuf, 0, sizeof(sbuf));
	length = readconfig(buf, rcnt, strdev, sbuf);
	config->devNum = strtoul(sbuf, 0, 10);

	printf("MainDevice IP = %s\n",config->maindeviceip);
	printf("MainDevice MAC = %s\n",config->maindevicemac);
	printf("MainDevice UUID = %s\n",config->maindeviceuuid);
	printf("MainDevice PORT = %d\n",config->maindeviceport);
	printf("USER IP = %s\n",config->userip);
	printf("USER PORT = %d\n",config->userport);
	printf("DEVNUM = %d\n",config->devNum);
	printf("My UUID = %s\n", config->myuuid);

	memset(sbuf,0,sizeof(sbuf));
}

