#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <openssl/aes.h>
#include <openssl/des.h>
//#include <pbc.h>
//#include <pbc_test.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <time.h>
#include <arpa/inet.h>
#include <openssl/hmac.h>
//#include <sqlite3.h>

#define DEV_ID 0x05

#define SERVERPORT 30005
#define SERVERBUFMAX 2048
#define SERVERSENDTIMEOUT 50000// 50ms
#define SERVERRECVTIMEOUT 50000// 50ms
#define SERVERRESPONSETIMEOUT 5000

#define CLIENTSENDTIMEOUT 1000000 //1s
#define CLIENTRECVTIMEOUT 2000000 //3s
#define MAXCLIENTBUFSIZE 8192
#define MAXNETBUFSIZE 8192*10

#define eprintfExit(x, ...) { fprintf(stderr, __VA_ARGS__); exit(x); }
#define eprintfReturn(x, ...) { fprintf(stderr, __VA_ARGS__); return (x); }


typedef enum{
	SESSION_CLOSE = 0,
	SESSION_CONNECTED = 1
}CLTSESSION_t;

typedef struct _SKMIB{
	int _main;
	int _userSetup;
	int _userAuth;
	int _neighbor;
	int _userAuthReq;
	int _neighborAuth;
}SKMIB_t;

typedef struct tCONFIG{
	char maindeviceip[16];
	char maindevicemac[20];
	char maindeviceuuid[40];
	char myuuid[40];
	char userip[20];
	int maindeviceport;
	int userport;
	int L2capport;
	int devNum;
}CONFIG_t;

typedef struct _AUTH2{
	int cnt_neighbor;
	int cnt_auth;
	int cnt_authed;
}AUTH_t;

typedef struct _CHECK{
	unsigned char MAC[20];
	unsigned char KEY[128];
	unsigned char nKEY[128];
	unsigned char CIPHER[260];
	unsigned char UUID[50];
	int socket;
	int devId;
	int psm;
	int result;
	CLTSESSION_t session;
}CHECK_t;

//aes.c
int aes_encrypt(unsigned char* in, unsigned char* out, int size, unsigned char* key, int KEY_BIT);
int aes_decrypt(unsigned char* in, unsigned char* out, int size, unsigned char* key, int KEY_BIT);
unsigned char* HMAC_SHA512(unsigned char *key, int keyLen, unsigned char *data, unsigned int *len2);

//sdp.c
sdp_session_t *register_service();
int sdp_search(int* psm);

//util.c
void config_init(CONFIG_t *config);
int _str2uuid(const char* uuid_str, uuid_t* uuid);
char* get_bt_addr();
char* get_bt_name();
char* get_bt_uuid();
void printf_byte(unsigned char* data, int len);
void concate_byte(unsigned char* out, unsigned char* in1, int in1Len, unsigned char *in2, int in2Len);
unsigned char* message_padding(unsigned char* data, int dataLen, int padding);

//bluetooth.c
void* bluetooth_thread(void *arg);

//bluetooth_l2cap.c
void* l2cap_thread(void *arg);
void l2cap_send_sock(unsigned char* data, int len, int sock);
void l2cap_send(unsigned char* data, int len);

//sym.c
unsigned char* genSymKey(int keyLen);

//network.c
void* network_thread(void *arg);

//network_sserver.c
void* tcp_server_thread(void *arg);
int server_tcpsend(uint8_t *data, int lenght);

//network_client.c
void* tcp_client_thread(void *arg);
int client_send(uint8_t *data, int lenght);
int client_disconnect(void);

//network_parse.c
void parse_role(unsigned char* data);
void parse_role_socket(unsigned char* data, int socket);

//network_packet.c
void sendnRegPacket();
void sendKeyPacket(unsigned char* _key, int keyLen);
void sendTag(unsigned char* data);
void sendEnc(char* ID, int index);

//save.c
void saveSymKey(unsigned char* data, int len);
void readSymKey();
void saveNeighborList(unsigned char* _key, int keyLen, unsigned char* _mac, int macLen, int devNum);
unsigned char* readNeighborList(int *len);

extern SKMIB_t skmib;
extern CONFIG_t cfg;
extern CHECK_t auth[8];
extern AUTH_t auth2;
extern int nCount;
extern unsigned char _nKEY[128];
