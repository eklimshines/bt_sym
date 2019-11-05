#include "main.h"

static int l2cap_sock = -1;
static CLTSESSION_t l2capsession = 0;

CLTSESSION_t l2cap_setsession(CLTSESSION_t session);
CLTSESSION_t l2cap_getsession(void);
CHECK_t auth[8];

void l2cap_setsock(int sock);
int l2cap_getsock();
void l2cap_close();

void l2cap_setsock(int sock){
	l2cap_sock = sock;
}
int l2cap_getsock(){
	return l2cap_sock;
}
void l2cap_close(){
	close(l2cap_sock);
}

CLTSESSION_t l2cap_setsession(CLTSESSION_t session){
    l2capsession = session;
    return l2capsession;
}

CLTSESSION_t l2cap_getsession(void){
    return l2capsession;
}

void l2cap_send(unsigned char* data, int len){
	int sock = l2cap_getsock();
	int status = 0;
	if(sock>0){
		status = write(sock, data, len);
		if(status < 0) printf("L2CAP Send Failed\n");
		else printf("L2CAP Send Success\n");
	}
}

CONFIG_t cfg;

int l2cap_connect(int psm){
    struct sockaddr_l2 addr = { 0 };
    int s, status;
    char *message = "hello!";
    //char dest[18] = "01:23:45:67:89:AB";
/*
    if(argc < 2)
    {
        fprintf(stderr, "usage: %s <bt_addr>\n", argv[0]);
        exit(2);
    }
*/
    //strncpy(dest, argv[1], 18);

    // allocate a socket
    s = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);

    // set the connection parameters (who to connect to)
    addr.l2_family = AF_BLUETOOTH;
    //addr.l2_psm = htobs(0x1001);
    //addr.l2_psm = htobs(psm);
    //str2ba( cfg.maindevicemac, &addr.l2_bdaddr );

    addr.l2_psm = htobs(auth[psm].psm);
    str2ba( auth[psm].MAC, &addr.l2_bdaddr );

    //printf("L2CAP MAC:%s %d %d\n", cfg.maindevicemac, psm, addr.l2_psm);
    printf("L2CAP MAC:%s %d %d\n", auth[psm].MAC, auth[psm].psm, addr.l2_psm);
    // connect to server
    status = connect(s, (struct sockaddr *)&addr, sizeof(addr));

    // send a message
    //if( status == 0 ) {
        //status = write(s, "hello!", 6);
	l2cap_setsock(s);
    //}

    if( status < 0 ) perror("uh oh");

    sendEnc(auth[psm].MAC, psm);

    //close(s);
    //
    return s;
}

void l2cap_rxthread(void *arg){
	int sock = 0;
	int result = 0; 
	char buf[MAXNETBUFSIZE] = {0};
	int len = 0;
	int i = 0;
	uint32_t timeout = 0;
	int plen, ptr =0;
	printf("Create L2CAP Rx Thread\n");

	while(1){
		sock = l2cap_getsock();
		if(sock>=0){
			memset(buf, 0, sizeof(buf));
			len = read(sock, buf, sizeof(buf));
			if(len>0){
				//printf_byte(buf, len);
				parse_role(buf);
			} else {
				usleep(1000); //1ms
			}
		} else {
		}
	}
	printf("Exit L2CAP Rx Thread\n");
}

void* l2cap_session_thread(void *arg){
	int sock = 0;
	int result = 0;
	int psm = *(int *)arg;
	char buf[MAXNETBUFSIZE] = {0};
	CLTSESSION_t session;

	printf("Create L2cap session Thread \n");

	while(1){
		session = l2cap_getsession();
		switch(session){
			case SESSION_CLOSE:
				result = l2cap_connect(psm);
				if(result > 0){
					l2cap_setsession(SESSION_CONNECTED);
				}
				break;
			case SESSION_CONNECTED:
				break;
		}
		
	}
}

void* l2cap_thread(void *arg){
	pthread_t l2capid;
	pthread_t l2cap_rxid;
	int ret = 0;
	int err = 0;
	int psm = *(int *)arg;

	printf("Create L2Cap Thread \n");

	err = pthread_create(&l2capid,NULL,l2cap_session_thread,(void *)&psm);
	if(err!=0) printf("Create L2cap session Thread Error[%d]\n",err);

	err = pthread_create(&l2cap_rxid,NULL,l2cap_rxthread,NULL);
	if(err!=0) printf("Create l2cap rx Thread Error[%d]\n",err);

	err = pthread_join(l2capid,(void**)&ret);
	if(err!=0) printf("l2cap session Thread Return[%d][%d]\n",err,ret);

	err = pthread_join(l2cap_rxid,(void**)&ret);
	if(err!=0) printf("l2cap rx Thread Return[%d][%d]\n",err,ret);

	l2cap_close();

}

